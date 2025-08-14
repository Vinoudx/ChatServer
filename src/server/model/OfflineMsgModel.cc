#include "OfflineMsgModel.hpp"

#include "db.hpp"

bool OfflineMsgModel::insert(int userid, const std::string& msg){
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO OfflineMessage VALUES (%d, '%s')", 
        userid, msg.c_str());

    Mysql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}

bool OfflineMsgModel::remove(int userid){
    char sql[1024] = {0};
    sprintf(sql, "delete from OfflineMessage where userid=%d", 
        userid);

    Mysql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}

std::vector<std::string> OfflineMsgModel::query(int userid){
    char sql[1024] = {0};
    sprintf(sql, "SELECT message FROM OfflineMessage WHERE userid=%d", userid);

    Mysql mysql; 
    std::vector<std::string> vec;
    if(mysql.connect()){
        MYSQL_RES* res = mysql.query(sql);   
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                vec.push_back(row[0]);
            }
        }
    }
    return vec;
}