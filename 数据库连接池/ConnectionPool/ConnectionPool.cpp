#include "ConnectionPool.h"
#include <iostream>
using namespace std;

//线程安全的懒汉单例
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool;
	return &pool;
}

//从配置文件加载配置项
bool ConnectionPool::loadConfigFile()
{
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr)
	{
		cout << "mysql.ini file is not exist!" << endl;
		return false;
	}
	while (!feof(pf))
	{
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int idx = str.find('=', 0);

		if (idx == -1)
		{
			continue;
		}
		
		int endidx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);

		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdelTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut")
		{
			_connectionTimeout = atoi(value.c_str());
		}
	}
	return true;
}

//构造函数
ConnectionPool::ConnectionPool()
{
	//加载配置项
	if (!loadConfigFile())
	{
		return;
	}
	//创建初始数量的连接
	for (int i = 0; i < _initSize; i++)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAlivetime(); //刷新开始空闲的起始时间
		_connectionQue.push(p);
		_connectionCnt++;	
	}

	//启动一个新的线程作为连接生产者
	thread produce(bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();//守护线程

	//启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，回收线程
	thread scanner(bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();//守护线程
}

// 运行在独立的线程中，专门产生新链接
void ConnectionPool::produceConnectionTask()
{
	for (;;)
	{
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			cv.wait(lock);
		}
		if (_connectionCnt < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAlivetime(); //刷新开始空闲的起始时间
			_connectionCnt++;
			_connectionQue.push(p);
		}

		cv.notify_all();
	}
}

// 给外部一个接口，从连接池中获取一个可用的空闲连接
shared_ptr<Connection> ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);
	while (_connectionQue.empty())
	{
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
		{
			if (_connectionQue.empty())
			{
				std::cout << "获取连接超时";
				return nullptr;
			}
		}
	}

	/*
	shared_ptr只能指针析构时，会deleteconnection资源，这里需要自定义释放资源的方式，把connection归还到que中
	*/
	shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection* pcon) {
			//服务器线程中调用，要考虑线程安全性
			unique_lock<mutex> lock(_queueMutex);
			pcon->refreshAlivetime(); //刷新开始空闲的起始时间
			_connectionQue.push(pcon);
		});
	_connectionQue.pop();
	cv.notify_all(); //消费完连接之后，通知生产者线程检查如果队列为空，开始生产连接。
	return sp;
}

void ConnectionPool::scannerConnectionTask()
{
	for (; ;)
	{
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));

		//扫描队列，释放多余连接
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAlivetime() >= _maxIdleTime * 1000)
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p;
			}
			else
			{
				break; //队头连接没有超过_maxIdleTime，其他肯定没超过
			}
		}
	}
}
