#pragma once
#include <mysql/mysql.h>
#include <string>
#include <ctime>
// 实现MySQL数据库的操作

class Connection
{
public:
    Connection();
    ~Connection();
    bool connect(std::string ip, unsigned short port, std::string user, std::string password, std::string dbname);
    bool update(std::string sql);
    MYSQL_RES *query(std::string sql);
    void refreshAliveTime() { _alivetime = clock(); }             // 刷新以下连接起始的空闲时间点
    clock_t getAliveTime() const { return clock() - _alivetime; } // 返回存活的时间
private:
    MYSQL *_conn;       // 表示和MySQL Server的一条连接
    clock_t _alivetime; // 记录进入空闲状态后的其实存活时间（进入队列）
};