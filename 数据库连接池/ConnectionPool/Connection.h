#pragma once
#include <mysql.h>
#include <string>
#include <ctime>
#include "public.h"
using namespace std;

// ���ݿ������

class Connection
{
public:
	// ��ʼ�����ݿ�����
	Connection();
	// �ͷ����ݿ�������Դ
	~Connection();
	// �������ݿ�
	bool connect(string ip, unsigned short port, string user, string password, string dbname);
	// ���²��� insert��delete��update
	bool update(string sql);
	// ��ѯ���� select
	MYSQL_RES* query(string sql);

	//ˢ��������ʼ�Ŀ���ʱ���
	void refreshAlivetime() { _alivetime = clock(); }

	//���ش��ʱ��
	clock_t getAlivetime() { return clock() - _alivetime; }

private:
	MYSQL* _conn; // ��ʾ��MySQL Server��һ������
	clock_t _alivetime; // ��¼�������״̬��Ĵ��ʱ��
};