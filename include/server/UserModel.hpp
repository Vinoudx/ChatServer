#ifndef _USER_MODEL_
#define _USER_MODEL_

#include "User.hpp"

class UserModel{
public:
    bool insert(User& user);
    User query(int id);
    bool updateState(User& user);
};


#endif