#ifndef _DB_
#define _DB_

#include <string>

#include <mysql/mysql.h>
#include <muduo/base/Logging.h>

static std::string server = "127.0.0.1";
static std::string user = "root";
static std::string password = "Fjh57593980";
static std::string dbname = "chat";

class Mysql{
public:
    Mysql(){
        m_conn = mysql_init(nullptr);
    }

    ~Mysql(){
        if(m_conn != nullptr){
            mysql_close(m_conn);
        }
    }

    bool connect(){
        MYSQL* p = mysql_real_connect(m_conn, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 3306, nullptr, 0);
        if(p != nullptr){
            mysql_query(m_conn, "set names gbk");
        }
    }

    bool update(const std::string& sql){
        if(mysql_query(m_conn, sql.c_str())){
            LOG_ERROR<< "update fail";
            return false;
        }
        return true;
    }

    MYSQL_RES* query(const std::string& sql){
        if(mysql_query(m_conn, sql.c_str())){
            LOG_ERROR<< "update fail";
            return nullptr;
        }
        return mysql_use_result(m_conn);
    }

private:
    MYSQL* m_conn;
};



#endif