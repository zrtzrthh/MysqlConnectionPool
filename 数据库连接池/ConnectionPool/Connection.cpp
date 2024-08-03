#include "Connection.h"
#include "public.h"
#include <iostream>
using namespace std;

Connection::Connection()
{
	//初始化数据库资源
	_conn = mysql_init(nullptr); 
}

Connection::~Connection()
{
	if (_conn != nullptr)
		mysql_close(_conn);
}

bool Connection::connect(string ip, unsigned short port, string user, string password, string dbname)
{
	//连接数据库
	MYSQL* p = mysql_real_connect(_conn, ip.c_str(), user.c_str(),
		password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}

bool Connection::update(string sql)
{
	//更新操作
	if (mysql_query(_conn, sql.c_str()))
	{
		cout << "更新失败:" + sql << endl;
		return false;
	}
	return true;
}

MYSQL_RES* Connection::query(string sql)
{
	//查询操作
	if (mysql_query(_conn, sql.c_str()))
	{
		cout << "查询失败:" + sql << endl;
		return nullptr;
	}
	return mysql_use_result(_conn);
}

