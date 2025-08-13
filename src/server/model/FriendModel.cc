#include "FriendModel.hpp"

#include "db.hpp"

bool FriendModel::insert(int userid, int friendid){
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO Friend VALUES (%d, %d)", userid, friendid);

    Mysql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}

std::vector<User> FriendModel::query(int userid){
    char sql[1024] = {0};
    sprintf(sql, "SELECT id, name, state FROM User, Friend WHERE friendid=id and userid=%d", userid);

    Mysql mysql;
    std::vector<User> vec;    
    if(mysql.connect()){
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                User u;
                u.setId(atoi(row[0]));
                u.setName(row[1]);
                u.setState(row[2]);
                vec.emplace_back(std::move(u));
            }
        }
    }
    return vec;
}