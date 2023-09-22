#pragma once
#include <queue>
#include <mutex>
#include "Connection.h"

// 实现连接池功能模块
class ConnectionPool
{
public:
    static ConnectionPool *getConnectionPool();

private:
    ConnectionPool() {} // 构造函数初始化
    ConnectionPool(ConnectionPool &other) = delete;
    ConnectionPool &operator=(ConnectionPool &other) = delete;

    void loadConfigFile();  // 从配置文件中加载配置项
    std::string _ip;        // mysql server的ip地址
    unsigned short _port;   // mysql server的端口号 3306
    std::string _username;  // mysql的用户名
    std::string _password;  // mysql的密码
    int _initSize;          // 连接池初始连接数量
    int _maxSize;           // 连接池的最大连接数量
    int _maxIdleTime;       // 连接池的最长空闲时间
    int _connectionTimeout; // 连接池获取连接的超时时间

    std::queue<Connection *> _connectionQue; // 存储mysql连接的队列
    std::mutex _queueMutex;                  // 维护连接队列的线程安全的互斥锁
};