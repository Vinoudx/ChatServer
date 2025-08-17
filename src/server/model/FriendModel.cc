#include "FriendModel.hpp"

#include "ConnectionPool.hpp"

bool FriendModel::insert(int userid, int friendid){
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO Friend VALUES (%d, %d)", userid, friendid);

    int id = ConnectionPool::instance()->update(sql);
    if(id == -1){
        LOG_ERROR<< "update fail " << sql;
        return false;
    }else{
        LOG_INFO<< "update success " << sql;
        return true;
    }
}

std::vector<User> FriendModel::query(int userid){
    char sql[1024] = {0};
    sprintf(sql, "select id, name, state from User, Friend where (friendid =id and userid = %d) or (userid = id and friendid= %d)", 
            userid, userid);

    std::vector<User> vec;
    MysqlRes res = ConnectionPool::instance()->query(sql);
    if(res.isValid){
        LOG_INFO<< "query success "<< sql;
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res.res)) != nullptr){
            User u;
            u.setId(atoi(row[0]));
            u.setName(row[1]);
            u.setState(row[2]);
            vec.emplace_back(std::move(u));
        }
    }else{
        LOG_ERROR<< "query fail "<<sql;
    }
    return vec;
}