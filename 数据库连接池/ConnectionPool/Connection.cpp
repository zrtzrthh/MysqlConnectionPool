#include "Connection.h"
#include "public.h"
#include <iostream>
using namespace std;

Connection::Connection()
{
	//��ʼ�����ݿ���Դ
	_conn = mysql_init(nullptr); 
}

Connection::~Connection()
{
	if (_conn != nullptr)
		mysql_close(_conn);
}

bool Connection::connect(string ip, unsigned short port, string user, string password, string dbname)
{
	//�������ݿ�
	MYSQL* p = mysql_real_connect(_conn, ip.c_str(), user.c_str(),
		password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}

bool Connection::update(string sql)
{
	//���²���
	if (mysql_query(_conn, sql.c_str()))
	{
		cout << "����ʧ��:" + sql << endl;
		return false;
	}
	return true;
}

MYSQL_RES* Connection::query(string sql)
{
	//��ѯ����
	if (mysql_query(_conn, sql.c_str()))
	{
		cout << "��ѯʧ��:" + sql << endl;
		return nullptr;
	}
	return mysql_use_result(_conn);
}

