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
	//��ȡ���ӳ�ʵ������
	static ConnectionPool* getConnectionPool();

	//���ⲿ�ṩ�ӿڣ������ӳػ�ȡһ�����õ����ӣ�ʹ������ָ�룬ʹ�����Ӻ�����ӷ��ص����ӳأ�������ֱ��������
	shared_ptr<Connection> getConnection();
private:
	ConnectionPool();

	//�������ļ�����������
	bool loadConfigFile();

	//ר�Ÿ�������������
	void produceConnectionTask();

	//ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ������߳�
	void scannerConnectionTask();

	string _ip; //mysql��ip��ַ
	unsigned short _port; //�˿�3306
	string _username; //�û���
	string _password; //����
	string _dbname; //���ӵ����ݿ�����
	int _initSize; //��ʼ��������
	int _maxSize; //���������
	int _maxIdleTime; //������ʱ��
	int _connectionTimeout; //���ӵȴ�ʱ��

	queue<Connection*> _connectionQue; //�洢mysql���ӵĶ���
	mutex _queueMutex; //ά�����Ӷ����̰߳�ȫ�Ļ�����
	atomic_int _connectionCnt; //��¼���ӳز�����connection�������̰߳�ȫ��

	condition_variable cv; //���������������������������߳������������̵߳�ͨ��
};

