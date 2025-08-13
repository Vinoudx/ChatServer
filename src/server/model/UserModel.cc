#include "UserModel.hpp"

#include "db.hpp"

bool UserModel::insert(User& user){
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO User(name, password, state) VALUES ('%s', '%s', '%s')", 
        user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    Mysql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            user.setId(mysql.getCurrentId());
            return true;
        }
    }
    return false;
}

User UserModel::query(int id){
    char sql[1024] = {0};
    sprintf(sql, "SELECT * FROM User WHERE id=%d", id);

    Mysql mysql;
    User user;    
    if(mysql.connect()){
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr){

                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                
                mysql_free_result(res);
            }
        }
    }
    return user;
}

bool UserModel::updateState(User& user){
    char sql[1024] = {0};
    sprintf(sql, "UPDATE User SET state='%s' WHERE id=%d", user.getState().c_str(), user.getId());

    Mysql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}

bool UserModel::resetState(){
    char sql[] = "UPDATE User SET state='offline' WHERE state='online'";

    Mysql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}