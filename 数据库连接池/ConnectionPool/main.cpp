#include<iostream>
#include"Connection.h"
#include"ConnectionPool.h"
using namespace std;

int main()
{
	Connection conn;
	conn.connect("127.0.0.1", 3306, "root", "1814338466", "chat");

	clock_t begin = clock();
		thread t1(
			[]() {
				for (int i = 0; i < 100; ++i)
				{
					Connection conn;
					char sql[1024] = { 0 };
					sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
					conn.connect("127.0.0.1", 3306, "root", "1814338466", "chat");
					conn.update(sql);
				}
			}
		);
		thread t2(
			[]() {
				for (int i = 0; i < 100; ++i)
				{
					Connection conn;
					char sql[1024] = { 0 };
					sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
					conn.connect("127.0.0.1", 3306, "root", "1814338466", "chat");
					conn.update(sql);
				}
			}
		);
		thread t3(
			[]() {
				for (int i = 0; i < 100; ++i)
				{
					Connection conn;
					char sql[1024] = { 0 };
					sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
					conn.connect("127.0.0.1", 3306, "root", "1814338466", "chat");
					conn.update(sql);
				}
			}
		);
		thread t4(
			[]() {
				for (int i = 0; i < 100; ++i)
				{
					Connection conn;
					char sql[1024] = { 0 };
					sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
					conn.connect("127.0.0.1", 3306, "root", "1814338466", "chat");
					conn.update(sql);
				}
			}
		);
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		clock_t end = clock();
		cout << end - begin << "ms" << endl;
}
	
//int main()
//{
//	clock_t begin = clock();
//	thread t1(
//		[]() {
//			ConnectionPool* cp = ConnectionPool::getConnectionPool();
//			for (int i = 0; i < 2500; ++i)
//			{
//				shared_ptr<Connection> sp = cp->getConnection();
//				char sql[1024] = { 0 };
//				sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
//				sp->update(sql);
//			}
//		}
//	);
//	thread t2(
//		[]() {
//			ConnectionPool* cp = ConnectionPool::getConnectionPool();
//			for (int i = 0; i < 2500; ++i)
//			{
//				shared_ptr<Connection> sp = cp->getConnection();
//				char sql[1024] = { 0 };
//				sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
//				sp->update(sql);
//			}
//		}
//	);
//	thread t3(
//		[]() {
//			ConnectionPool* cp = ConnectionPool::getConnectionPool();
//			for (int i = 0; i < 2500; ++i)
//			{
//				shared_ptr<Connection> sp = cp->getConnection();
//				char sql[1024] = { 0 };
//				sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
//				sp->update(sql);
//			}
//		}
//	);
//	thread t4(
//		[]() {
//			ConnectionPool* cp = ConnectionPool::getConnectionPool();
//			for (int i = 0; i < 2500; ++i)
//			{
//				shared_ptr<Connection> sp = cp->getConnection();
//				char sql[1024] = { 0 };
//				sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
//				sp->update(sql);
//			}
//		}
//	);
//	t1.join();
//	t2.join();
//	t3.join();
//	t4.join();
//	
//	clock_t end = clock();
//	cout << end - begin << "ms" << endl;
//}




