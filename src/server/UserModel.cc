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