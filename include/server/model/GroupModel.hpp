#ifndef _GROUP_MODEL_
#define _GROUP_MODEL_

#include <string>
#include <vector>

#include "Group.hpp"

class GroupModel{
public:
    bool createGroup(Group& group);

    bool addGroup(int userid, int groupid, const std::string& role);

    // 获得用户所有有关组
    std::vector<Group> queryGroups(int userid);

    // 获得除调用者之外的所有组内用户id
    std::vector<int> queryMembers(int userid, int groupid);

};

#endif