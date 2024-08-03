#include "ConnectionPool.h"
#include <iostream>
using namespace std;

//�̰߳�ȫ����������
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool;
	return &pool;
}

//�������ļ�����������
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

//���캯��
ConnectionPool::ConnectionPool()
{
	//����������
	if (!loadConfigFile())
	{
		return;
	}
	//������ʼ����������
	for (int i = 0; i < _initSize; i++)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAlivetime(); //ˢ�¿�ʼ���е���ʼʱ��
		_connectionQue.push(p);
		_connectionCnt++;	
	}

	//����һ���µ��߳���Ϊ����������
	thread produce(bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();//�ػ��߳�

	//����һ���µĶ�ʱ�̣߳�ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ������߳�
	thread scanner(bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();//�ػ��߳�
}

// �����ڶ������߳��У�ר�Ų���������
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
			p->refreshAlivetime(); //ˢ�¿�ʼ���е���ʼʱ��
			_connectionCnt++;
			_connectionQue.push(p);
		}

		cv.notify_all();
	}
}

// ���ⲿһ���ӿڣ������ӳ��л�ȡһ�����õĿ�������
shared_ptr<Connection> ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);
	while (_connectionQue.empty())
	{
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
		{
			if (_connectionQue.empty())
			{
				std::cout << "��ȡ���ӳ�ʱ";
				return nullptr;
			}
		}
	}

	/*
	shared_ptrֻ��ָ������ʱ����deleteconnection��Դ��������Ҫ�Զ����ͷ���Դ�ķ�ʽ����connection�黹��que��
	*/
	shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection* pcon) {
			//�������߳��е��ã�Ҫ�����̰߳�ȫ��
			unique_lock<mutex> lock(_queueMutex);
			pcon->refreshAlivetime(); //ˢ�¿�ʼ���е���ʼʱ��
			_connectionQue.push(pcon);
		});
	_connectionQue.pop();
	cv.notify_all(); //����������֮��֪ͨ�������̼߳���������Ϊ�գ���ʼ�������ӡ�
	return sp;
}

void ConnectionPool::scannerConnectionTask()
{
	for (; ;)
	{
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));

		//ɨ����У��ͷŶ�������
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
				break; //��ͷ����û�г���_maxIdleTime�������϶�û����
			}
		}
	}
}
