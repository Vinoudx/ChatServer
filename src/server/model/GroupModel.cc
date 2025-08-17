#include "GroupModel.hpp"

#include "ConnectionPool.hpp"

bool GroupModel::createGroup(Group& group){
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO AllGroup(groupname, groupdesc) VALUES ('%s', '%s')", group.getName().c_str(), group.getDesc().c_str());

    int id = ConnectionPool::instance()->update(sql);
    if(id == -1){
        LOG_ERROR<< "update fail "<<sql;
        return false;
    }else{
        group.setId(id);
        LOG_INFO<< "update success "<<sql;
        return true;
    }
}

bool GroupModel::addGroup(int userid, int groupid, const std::string& role){
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO GroupUser VALUES (%d, %d, '%s')", groupid, userid, role.c_str());

    int id = ConnectionPool::instance()->update(sql);
    if(id == -1){
        LOG_ERROR<< "update fail "<<sql;
        return false;
    }else{
        LOG_INFO<< "update success "<<sql;
        return true;
    }
}

std::vector<Group> GroupModel::queryGroups(int userid){
    char sql[1024] = {0};
    sprintf(sql, "SELECT a.id, a.groupname, a.groupdesc FROM AllGroup a inner join GroupUser b WHERE a.id=b.groupid and b.userid=%d", userid);

    std::vector<Group> vec;
    MysqlRes res = ConnectionPool::instance()->query(sql);
    if(res.isValid){
        LOG_INFO<< "query success "<<sql;
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res.res)) != nullptr){
            Group u;
            u.setId(atoi(row[0]));
            u.setName(row[1]);
            u.setDesc(row[2]);
            vec.emplace_back(std::move(u));
        }
    }else{
        LOG_ERROR<<"query fail "<<sql;
    }
    return vec;
}

// 获得除调用者之外的所有组内用户id
std::vector<int> GroupModel::queryMembers(int userid, int groupid){
    char sql[1024] = {0};
    sprintf(sql, "SELECT userid FROM GroupUser WHERE groupid=%d and userid!=%d", groupid, userid);

    std::vector<int> vec;
    MysqlRes res = ConnectionPool::instance()->query(sql);
    if(res.isValid){
        LOG_INFO<< "query success "<<sql;
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res.res)) != nullptr){
            vec.push_back(atoi(row[0]));
        }
    }else{
        LOG_ERROR<<"query fail "<<sql;
    }
    return vec;
}