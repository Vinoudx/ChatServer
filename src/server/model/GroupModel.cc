#include "GroupModel.hpp"

#include "db.hpp"

bool GroupModel::createGroup(Group& group){
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO AllGroup(groupname, groupdesc) VALUES ('%s', '%s')", group.getName().c_str(), group.getDesc().c_str());

    Mysql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            group.setId(mysql.getCurrentId());
            return true;
        }
    }
    return false;
}

bool GroupModel::addGroup(int userid, int groupid, const std::string& role){
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO GroupUser VALUES (%d, %d, '%s')", groupid, userid, role.c_str());

    Mysql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}

std::vector<Group> GroupModel::queryGroups(int userid){
    char sql[1024] = {0};
    sprintf(sql, "SELECT a.id, a.groupname, a.groupdesc FROM AllGroup a inner join GroupUser b WHERE a.id=b.groupid and b.userid=%d", userid);

    Mysql mysql;
    std::vector<Group> vec;    
    if(mysql.connect()){
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                Group u;
                u.setId(atoi(row[0]));
                u.setName(row[1]);
                u.setDesc(row[2]);
                vec.emplace_back(std::move(u));
            }
        }
    }
    return vec;
}

// 获得除调用者之外的所有组内用户id
std::vector<int> GroupModel::queryMembers(int userid, int groupid){
    char sql[1024] = {0};
    sprintf(sql, "SELECT userid FROM GroupUser WHERE groupid=%d and userid!=%d", groupid, userid);

    Mysql mysql;
    std::vector<int> vec;    
    if(mysql.connect()){
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                vec.push_back(atoi(row[0]));
            }
        }
    }
    return vec;
}