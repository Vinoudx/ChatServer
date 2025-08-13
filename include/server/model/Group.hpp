#ifndef _GROUP_
#define _GROUP_

#include <string>
#include <vector>

#include "User.hpp"

class Group{
public:
    int getId(){return m_id;}
    std::string getName(){return m_name;}
    std::string getDesc(){return m_desc;}
    const std::vector<User>& getUsers(){return m_users;}

    void setId(int id){m_id = id;}
    void setName(const std::string& name){m_name = name;}
    void setDesc(const std::string& desc){m_desc = desc;}
    void setUsers(const std::vector<User>& users){m_users = users;}
    void setUsers(std::vector<User>&& users){m_users = std::move(users);}

private:
    int m_id;
    std::string m_name;
    std::string m_desc;
    std::vector<User> m_users;
};

#endif