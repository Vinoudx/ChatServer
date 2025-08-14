#ifndef _GROUP_
#define _GROUP_

#include <string>
#include <vector>

#include "User.hpp"
#include "GroupUser.hpp"

class Group{
public:
    int getId(){return m_id;}
    std::string getName(){return m_name;}
    std::string getDesc(){return m_desc;}
    std::vector<GroupUser>& getUsers(){return m_users;}

    void setId(int id){m_id = id;}
    void setName(const std::string& name){m_name = name;}
    void setDesc(const std::string& desc){m_desc = desc;}

private:
    int m_id;
    std::string m_name;
    std::string m_desc;
    std::vector<GroupUser> m_users;
};

#endif