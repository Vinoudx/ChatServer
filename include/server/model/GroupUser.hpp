#ifndef _GROUP_USER_
#define _GROUP_USER_

#include <string>

#include "User.hpp"

class GroupUser : public User{
public:
    std::string getRole(){return m_role;}
    void setRole(const std::string& role){m_role = role;}
private:
    std::string m_role;
};

#endif