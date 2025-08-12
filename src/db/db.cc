#include "db.hpp"

#include <muduo/base/Logging.h>

Mysql::Mysql(){
    m_conn = mysql_init(nullptr);
}

Mysql::~Mysql(){
    if(m_conn != nullptr){
        mysql_close(m_conn);
    }
}

bool Mysql::connect(){
    MYSQL* p = mysql_real_connect(m_conn, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if(p != nullptr){
        mysql_query(m_conn, "set names gbk");
        return true;
    }else{
        LOG_ERROR<< "mysql connection fail " << mysql_error(m_conn);
    }
    return false;
}

bool Mysql::update(const std::string& sql){
    if(mysql_query(m_conn, sql.c_str())){
        LOG_ERROR<< "update fail";
        return false;
    }
    return true;
}

MYSQL_RES* Mysql::query(const std::string& sql){
    if(mysql_query(m_conn, sql.c_str())){
        LOG_ERROR<< "update fail";
        return nullptr;
    }
    return mysql_use_result(m_conn);
}

int Mysql::getCurrentId(){
    return mysql_insert_id(m_conn);
}
