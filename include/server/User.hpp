#ifndef _USER_
#define _USER_

#include <string>

class User{
public:
    User(int id = -1, std::string name = "", std::string password = "", std::string state = "offline"):
        m_id(id),
        m_name(name),
        m_password(password),
        m_state(state){};

    int getId(){return m_id;}
    std::string getName(){return m_name;}
    std::string getPwd(){return m_password;}
    std::string getState(){return m_state;}

    void setId(int id){m_id = id;}
    void setName(const std::string& name){m_name = name;}
    void setPwd(const std::string& pwd){m_password = pwd;}
    void setState(const std::string& state){m_state = state;}

private:
    int m_id;
    std::string m_name;
    std::string m_password;
    std::string m_state;
};

#endif