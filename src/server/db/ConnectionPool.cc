#include "ConnectionPool.hpp"

MysqlRes::~MysqlRes(){
    if(res != nullptr){
        mysql_free_result(res);
    }
}

void MysqlRes::setRes(MYSQL_RES* r){
    res = r;
}

Connections::Connections():isRunning(false){
    m_con = mysql_init(nullptr);
    MYSQL* p = mysql_real_connect(m_con, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if(p != nullptr){
        LOG_INFO << "a mysql connection established";
    }else{
        LOG_ERROR<< "mysql connection fail " << mysql_error(m_con);
    }
}

Connections::~Connections(){
    if(m_con != nullptr){
        mysql_close(m_con);
    }
}

ConnectionPool::ConnectionPool():m_total(num_connections),m_frees(num_connections){
    m_cons.reserve(num_connections);
    for(int i=0; i<num_connections; i++){
        m_cons.emplace_back();
    }
}

int ConnectionPool:: findFirstFree(){
    for(int i=0; i<m_total; i++){
        if(!m_cons.at(i).isRunning){
            return i;
        }
    }
    return 0; // 不会走到这里
}

int ConnectionPool::update(const char* commands){
    // 首先条件变量观察是否有空闲连接
    std::unique_lock<std::mutex> l(m_lock);
    m_cv.wait(l, [this]{return m_frees > 0;});
    // 找到一个空闲的连接
    int index = findFirstFree();
    m_cons.at(index).isRunning = true;
    l.unlock();
    m_frees--;
    // 执行sql
    LOG_INFO<< "excuting: " << commands;
    int id = 0;
    if(!mysql_query(m_cons.at(index).m_con, commands)){
        id = mysql_insert_id(m_cons.at(index).m_con);
    }else{
        id = -1;
    }
    // 执行完放回连接
    l.lock();
    m_cons.at(index).isRunning = false;
    l.unlock();
    m_frees++;
    m_cv.notify_all();
    return id;
}

MysqlRes ConnectionPool::query(const char* commands){
    // 首先条件变量观察是否有空闲连接
    std::unique_lock<std::mutex> l(m_lock);
    m_cv.wait(l, [this]{return m_frees > 0;});
    // 找到一个空闲的连接
    int index = findFirstFree();
    m_cons.at(index).isRunning = true;
    l.unlock();
    m_frees--;
    // 执行sql
    LOG_INFO<< "excuting: " << commands;
    MysqlRes res(nullptr);
    if(!mysql_query(m_cons.at(index).m_con, commands)){
        res.setRes(mysql_use_result(m_cons.at(index).m_con));
    }else{
        res.isValid = false;
    }
    // 执行完放回连接
    l.lock();
    m_cons.at(index).isRunning = false;
    l.unlock();
    m_frees++;
    m_cv.notify_all();
    return res;
}