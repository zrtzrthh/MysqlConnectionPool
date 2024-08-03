#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>
#include "Connection.h"
using namespace std;

class ConnectionPool
{
public:
	//获取连接池实例对象
	static ConnectionPool* getConnectionPool();

	//给外部提供接口，从连接池获取一个可用的连接，使用智能指针，使用连接后把连接返回到连接池，而不是直接析构掉
	shared_ptr<Connection> getConnection();
private:
	ConnectionPool();

	//从配置文件加载配置项
	bool loadConfigFile();

	//专门负责生产新连接
	void produceConnectionTask();

	//扫描超过maxIdleTime时间的空闲连接，回收线程
	void scannerConnectionTask();

	string _ip; //mysql的ip地址
	unsigned short _port; //端口3306
	string _username; //用户名
	string _password; //密码
	string _dbname; //连接的数据库名称
	int _initSize; //初始的连接量
	int _maxSize; //最大连接量
	int _maxIdleTime; //最大空闲时间
	int _connectionTimeout; //连接等待时间

	queue<Connection*> _connectionQue; //存储mysql连接的队列
	mutex _queueMutex; //维护连接队列线程安全的互斥锁
	atomic_int _connectionCnt; //记录连接池产生的connection总量，线程安全的

	condition_variable cv; //设置条件变量，用于连接生产线程与连接消费线程的通信
};

