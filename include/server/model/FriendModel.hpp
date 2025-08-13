#ifndef _FRIEND_MODEL_
#define _FRIEND_MODEL_

#include <vector>

#include "User.hpp"

class FriendModel{
public:
    bool insert(int userid, int friendid);
    std::vector<User> query(int userid);
};

#endif