#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <vector>
#include <condition_variable>
#include <mutex>
#include <atomic>

#include <mysql/mysql.h>
#include <muduo/base/Logging.h>

#include "public.hpp"

static std::string server = "127.0.0.1";
static std::string user = "root";
static std::string password = "Fjh57593980";
static std::string dbname = "chat";
static size_t num_connections = 8;

// g++ -std=c++20 ./main.cc ConnectionPool.cc -l muduo_base -l mysqlclient -l mysqlcppconn -l pthread

struct MysqlRes{
    MysqlRes(MYSQL_RES* r):res(r),isValid(true){}
    MysqlRes(const MysqlRes&) = delete;
    MysqlRes& operator=(const MysqlRes&) = delete;
    MysqlRes(MysqlRes&&) = default;
    MysqlRes& operator=(MysqlRes&&) = default;
    ~MysqlRes();
    void setRes(MYSQL_RES* r);
    MYSQL_RES* res;
    bool isValid;
};

struct Connections{
    Connections();
    Connections(const Connections&) = delete;
    Connections(Connections&&) = default;
    ~Connections();
    bool isRunning;
    MYSQL* m_con;
};


class ConnectionPool : public Singleton<ConnectionPool>{
    friend class Singleton<ConnectionPool>;
public:
    ConnectionPool();

    // 要返回更新后的key
    int update(const char* commands);

    MysqlRes query(const char* commands);
private:

    // 能进这个函数则一定有空闲连接
    int findFirstFree();

    // 放置连接
    std::vector<Connections> m_cons;
    // 连接池的互斥锁和条件变量
    std::mutex m_lock;
    std::condition_variable m_cv;

    // 空闲连接数量
    std::atomic_uint64_t m_frees;
    // 总连接数量
    size_t m_total; 
};

#endif