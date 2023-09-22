#include "CommonConnectionPool.h"

// 线程安全的懒汉单例模式函数接口
ConnectionPool *ConnectionPool::getConnectionPool()
{
    // 静态局部变量的初始化，编译器自动lock和unlock
    static ConnectionPool pool;
    return &pool;
}

// 连接池的构造
ConnectionPool::ConnectionPool() : _connectionCnt(0)
{
    // 加载配置项
    if (!loadConfigFile())
    {
        return;
    }
    // 创建初始书数量的连接
    for (int i = 0; i < _initSize; i++)
    {
        Connection *p = new Connection();
        p->connect(_ip, _port, _username, _password, _dbname);
        p->refreshAliveTime(); // 刷新进入队列的起始时间
        _connectionQue.push(p);
        _connectionCnt++;
    }
    // 创建一个生产者线程，生产连接
    std::thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();

    // 创建一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，对多余的连接进行回收
    std::thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

void ConnectionPool::produceConnectionTask()
{
    for (;;)
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        while (!_connectionQue.empty())
        {
            cv.wait(lock); // 队列不空，此处生产线程进入等待状态
        }

        // 连接数量没有到达上限，继续生产连接
        if (_connectionCnt < _maxSize)
        {
            Connection *p = new Connection();
            p->connect(_ip, _port, _username, _password, _dbname);
            p->refreshAliveTime();
            _connectionQue.push(p);
            _connectionCnt++;
        }
        // 通知消费者线程可以消费连接
        cv.notify_all();
    }
}

std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_queueMutex);
    if (_connectionQue.empty())
    {

        std::cv_status status = cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeout));
        if (_connectionQue.empty() && status == std::cv_status::timeout)
        {
            LOG("获取空闲连接超时了，获取链接失败");
            return nullptr;
        }
    }

    // shared_ptr智能指针析构时，会把connection资源直接delete掉，相当于调用connection的析构函数，connection就被close掉了，这里需要自定义shared_ptr的资源释放方式，把connection归还到queue当中
    // 自定义智能指针的资源释放方式
    std::shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection *pcon)
                                   {
        //这是在服务器应用线程中调用的，所以一定要考虑队列的线程安全操作
        std::unique_lock lock(_queueMutex);
        pcon->refreshAliveTime();
        _connectionQue.push(pcon); });
    _connectionQue.pop();
    cv.notify_all(); // 消费完连接以后，通知生产者线程检查一下，如果队列为空，赶紧生产连接；
    return sp;
}

// 运行在独立的线程中，专门扫描空闲连接的存活时间
void ConnectionPool::scannerConnectionTask()
{
    for (;;)
    {
        // 通过sleep模拟定时效果
        std::this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));
        // 扫描整个队列释放多余的连接
        std::unique_lock<std::mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize)
        {
            Connection *p = _connectionQue.front();
            if (p->getAliveTime() >= _maxIdleTime * 1000)
            {
                _connectionQue.pop();
                _connectionCnt--;
                delete p;
            }
            else
            {
                break; // 队头的连接没有超过_maxIdletime，其他连接肯定没有
            }
        }
    }
}

// 从配置文件中加载配置项
bool ConnectionPool::loadConfigFile()
{
    FILE *pf = fopen("mysql.conf", "r");
    if (pf == nullptr)
    {
        LOG("mysql.conf file does not exist!");
        std::cout << errno << std::endl;
        return false;
    }
    while (!feof(pf))
    {
        char line[1024] = {0};
        fgets(line, 1024, pf);
        std::string str = line;
        int idx = str.find('=', 0);
        if (idx == -1) // 无效的配置项
        {
            continue;
        }
        int endidx = str.find('\n', idx);
        std::string key = str.substr(0, idx);
        std::string value = str.substr(idx + 1, endidx - idx - 1);
        if (key == "ip")
        {
            _ip = value;
        }
        else if (key == "port")
        {
            _port = std::stoi(value);
        }
        else if (key == "username")
        {
            _username = value;
        }
        else if (key == "password")
        {
            _password = value;
        }
        else if (key == "dbname")
        {
            _dbname = value;
        }
        else if (key == "initSize")
        {
            _initSize = std::stoi(value);
        }
        else if (key == "maxSize")
        {
            _maxSize = std::stoi(value);
        }
        else if (key == "maxIdleTime")
        {
            _maxIdleTime = std::stoi(value);
        }
        else if (key == "connectionTimeout")
        {
            _connectionTimeout = std::stoi(value);
        }
    }
    return true;
}
