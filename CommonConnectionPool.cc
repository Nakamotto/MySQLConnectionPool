#include "CommonConnectionPool.h"

// 线程安全的懒汉单例模式函数接口
ConnectionPool *ConnectionPool::getConnectionPool()
{
    // 静态局部变量的初始化，编译器自动lock和unlock
    static ConnectionPool pool;
    return &pool;
}

// 从配置文件中加载配置项
void ConnectionPool::loadConfigFile()
{
}