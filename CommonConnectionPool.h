#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <thread>
#include <functional>
#include <chrono>
#include "Connection.h"
#include "public.h"

// 实现连接池功能模块
class ConnectionPool
{
public:
    // 获取连接池对象实例
    static ConnectionPool *getConnectionPool();
    // 给外部提供接口，从连接池中获取一个可用的空闲连接，智能指针自动管理连接资源
    std::shared_ptr<Connection> getConnection();

private:
    ConnectionPool(); // 构造函数初始化
    ConnectionPool(ConnectionPool &other) = delete;
    ConnectionPool &operator=(ConnectionPool &other) = delete;
    bool loadConfigFile();        // 从配置文件中加载配置项
    void produceConnectionTask(); // 运行在独立的线程中，专门生产新连接
    void scannerConnectionTask(); // 运行在独立的线程中，专门扫描空闲连接的存活时间

    std::string _ip;        // mysql server的ip地址
    unsigned short _port;   // mysql server的端口号 3306
    std::string _username;  // mysql的用户名
    std::string _password;  // mysql的密码
    std::string _dbname;    // mysql的密码
    int _initSize;          // 连接池初始连接数量
    int _maxSize;           // 连接池的最大连接数量
    int _maxIdleTime;       // 连接池的最长空闲时间
    int _connectionTimeout; // 连接池获取连接的超时时间

    std::queue<Connection *> _connectionQue; // 存储mysql连接的队列
    std::mutex _queueMutex;                  // 维护连接队列的线程安全的互斥锁
    std::atomic_int _connectionCnt;          // 记录连接所创建的connection连接的总数量，基于CAS原理，线程安全
    std::condition_variable cv;              // 设置条件变量，用于连接生产线程和连接消费线程的通信
};