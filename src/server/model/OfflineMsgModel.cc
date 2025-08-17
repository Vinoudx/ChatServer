#include "OfflineMsgModel.hpp"

#include "ConnectionPool.hpp"

bool OfflineMsgModel::insert(int userid, const std::string& msg){
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO OfflineMessage VALUES (%d, '%s')", 
        userid, msg.c_str());

    int id = ConnectionPool::instance()->update(sql);
    if(id == -1){
        LOG_ERROR<< "update fail "<<sql;
        return false;
    }else{
        LOG_INFO<< "update success "<<sql;
        return true;
    }
}

bool OfflineMsgModel::remove(int userid){
    char sql[1024] = {0};
    sprintf(sql, "delete from OfflineMessage where userid=%d", 
        userid);

    int id = ConnectionPool::instance()->update(sql);
    if(id == -1){
        LOG_ERROR<< "update fail "<<sql;
        return false;
    }else{
        LOG_INFO<< "update success "<<sql;
        return true;
    }
}

std::vector<std::string> OfflineMsgModel::query(int userid){
    char sql[1024] = {0};
    sprintf(sql, "SELECT message FROM OfflineMessage WHERE userid=%d", userid);

    std::vector<std::string> vec;
    MysqlRes res = ConnectionPool::instance()->query(sql);
    if(res.isValid){
        LOG_INFO<<"query success "<<sql;
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res.res)) != nullptr){
            vec.push_back(row[0]);
        }
    }else{
        LOG_ERROR<<"query fail "<<sql;
    }
    return vec;
}