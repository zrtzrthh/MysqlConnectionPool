#include<iostream>
#include"Connection.h"
using namespace std;

int main()
{
	clock_t begin = clock();
	for (int i = 0; i < 1000; ++i)
	{
		Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
		conn.connect("127.0.0.1", 3306, "root", "1814338466", "chat");
		conn.update(sql);
	}
	clock_t end = clock();
	cout << end - begin << "ms" << endl;
}