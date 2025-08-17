#include "UserModel.hpp"

#include "ConnectionPool.hpp"

bool UserModel::insert(User& user){
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO User(name, password, state) VALUES ('%s', '%s', '%s')", 
        user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    int id = ConnectionPool::instance()->update(sql);
    if(id == -1){
        LOG_ERROR<< "update fail "<<sql;
        return false;
    }else{
        user.setId(id);
        LOG_INFO<< "update success "<<sql;
        return true;
    }
}

User UserModel::query(int id){
    char sql[1024] = {0}; 
    sprintf(sql, "SELECT * FROM User WHERE id=%d", id);

    User user;
    MysqlRes res = ConnectionPool::instance()->query(sql);
    if(res.isValid){
        LOG_INFO<< "query sueecss "<<sql;
        MYSQL_ROW row = mysql_fetch_row(res.res);
        if(row != nullptr){
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setPwd(row[2]);
            user.setState(row[3]); 
        }
    }else{
        LOG_ERROR<< "query fail "<<sql;
    }
    return user;
}

bool UserModel::updateState(User& user){
    char sql[1024] = {0};
    sprintf(sql, "UPDATE User SET state='%s' WHERE id=%d", user.getState().c_str(), user.getId());

    int id = ConnectionPool::instance()->update(sql);
    if(id == -1){
        LOG_ERROR<< "update fail "<<sql;
        return false;
    }else{
        LOG_INFO<< "update success "<<sql;
        return true;
    }
}

bool UserModel::resetState(){
    char sql[] = "UPDATE User SET state='offline' WHERE state='online'";

    int id = ConnectionPool::instance()->update(sql);
    if(id == -1){
        LOG_ERROR<< "update fail "<<sql;
        return false;
    }else{
        LOG_INFO<< "update success "<<sql;
        return true;
    }
}