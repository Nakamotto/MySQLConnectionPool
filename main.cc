#include <iostream>
#include "pch.h"
#include "Connection.h"
#include "CommonConnectionPool.h"
int main()
{

    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, age, sex) values('%s', '%d', '%s')", "zhang san", 20, "male");
    clock_t begin = clock();
    ConnectionPool *cp = ConnectionPool::getConnectionPool();

    std::thread t1([&]()
                   {
        for (int i = 0; i < 2500; ++i)
    {

        // Connection conn;

        // conn.connect("127.0.0.1", 3306, "root", "343316184", "chat");
        // conn.update(sql);
        ConnectionPool *cp = ConnectionPool::getConnectionPool();

        std::shared_ptr<Connection> sp = cp->getConnection();
        sp->update(sql);
    } });
    std::thread t2([&]()
                   {
        for (int i = 0; i < 2500; ++i)
    {
        // Connection conn;

        // conn.connect("127.0.0.1", 3306, "root", "343316184", "chat");
        // conn.update(sql);
        ConnectionPool *cp = ConnectionPool::getConnectionPool();

        std::shared_ptr<Connection> sp = cp->getConnection();
        sp->update(sql);
    } });
    std::thread t3([&]()
                   {
        for (int i = 0; i < 2500; ++i)
    {
        // Connection conn;

        // conn.connect("127.0.0.1", 3306, "root", "343316184", "chat");
        // conn.update(sql);
        ConnectionPool *cp = ConnectionPool::getConnectionPool();

        std::shared_ptr<Connection> sp = cp->getConnection();
        sp->update(sql);
    } });

    std::thread t4([&]()
                   {
        for (int i = 0; i < 2500; ++i)
    {
        // Connection conn;

        // conn.connect("127.0.0.1", 3306, "root", "343316184", "chat");
        // conn.update(sql);
        ConnectionPool *cp = ConnectionPool::getConnectionPool();

        std::shared_ptr<Connection> sp = cp->getConnection();
        sp->update(sql);
    } });
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    clock_t end = clock();
    std::cout << (end - begin) / (double)CLOCKS_PER_SEC * 1000 << "ms" << std::endl;
    return 0;
}