#ifndef _OFFLINE_MSG_MODEL_
#define _OFFLINE_MSG_MODEL_

#include <string>
#include <vector>

class OfflineMsgModel{
public:
    bool insert(int userid, const std::string& msg);
    bool remove(int userid);
    std::vector<std::string> query(int userid);
};

#endif