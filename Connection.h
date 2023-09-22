#pragma once
#include <mysql/mysql.h>
#include <string>
// 实现MySQL数据库的操作

class Connection
{
public:
    Connection();
    ~Connection();
    bool connect(std::string ip, unsigned short port, std::string user, std::string password, std::string dbname);
    bool update(std::string sql);
    MYSQL_RES* query(std::string sql);
private:
    MYSQL *_conn; //表示和MySQL Server的一条连接
};