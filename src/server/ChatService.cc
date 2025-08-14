#include "ChatService.hpp"

#include <vector>

#include <muduo/base/Logging.h>

#include "User.hpp"
#include "Group.hpp"
#include "GroupUser.hpp"

template<typename T = void>
void ChatService::registeHandler(EnMsgType msgtype, auto&& handler){
    m_cbFuncMap.insert_or_assign(getEnumValue(msgtype), 
        [handler = std::forward<decltype(handler)>(handler)](auto&& ...args)->void{
            handler(std::forward<decltype(args)>(args)...);
        }
    );
}

template <typename Obj>
void ChatService::registeHandler(EnMsgType msgtype, Obj* obj, void (Obj::*method)(const muduo::net::TcpConnectionPtr&, json&, muduo::Timestamp)) {
    m_cbFuncMap.insert_or_assign(
        getEnumValue(msgtype),
        [obj, method](auto&&... args) {
            (obj->*method)(std::forward<decltype(args)>(args)...);
        }
    );
}

ChatService::ChatService(){
    registeHandler<ChatService>(EnMsgType::MSG_LOGIN, this, &ChatService::login);
    registeHandler<ChatService>(EnMsgType::MSG_REG, this, &ChatService::reg);
    registeHandler<ChatService>(EnMsgType::MSG_ONE_CHAT, this, &ChatService::onechat);
    registeHandler<ChatService>(EnMsgType::MSG_ADD_FRIEND, this, &ChatService::addfriend);
    registeHandler<ChatService>(EnMsgType::MSG_CREATE_GROUP, this, &ChatService::creategroup);
    registeHandler<ChatService>(EnMsgType::MSG_JOIN_GROUP, this, &ChatService::joingroup);
    registeHandler<ChatService>(EnMsgType::MSG_GROUP_CHAT, this, &ChatService::groupchat);
    registeHandler<ChatService>(EnMsgType::MSG_LOGOUT, this, &ChatService::logout);
}

CbType ChatService::getHandler(MsgUnderType id){
    auto it = m_cbFuncMap.find(id);
    if(it == m_cbFuncMap.end()){
        // 返回默认处理器
        return [id](const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp)->void{
            LOG_ERROR << "missing handler of message id: " << id;
        };
    }else{
        return m_cbFuncMap[id];
    }
}

CbType ChatService::getHandler(EnMsgType msgtype){
    return getHandler(getEnumValue(msgtype));
}


void ChatService::login(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp){
    int id = js["id"];
    std::string password = js["pwd"];
    User user = m_usermodel.query(id);

    json response;
    if(user.getId() == id && user.getPwd() == password){

        if(user.getState() == "online"){
            response["msgid"] = getEnumValue(EnMsgType::MSG_LOGIN_ACK);
            response["errno"] = 1;
            response["errinfo"] = "already online";
            conn->send(response.dump());
        }else{
            user.setState("online");

            {
                std::lock_guard<std::mutex> l(m_conMutex);
                m_userConMap.insert_or_assign(user.getId(), conn);
            }

            response["msgid"] = getEnumValue(EnMsgType::MSG_LOGIN_ACK);
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 查询是否有离线消息
            std::vector<std::string> offlineMessage = m_offlinemsgmoodel.query(user.getId());
            if(!offlineMessage.empty()){
                response["offlinemsg"] = offlineMessage;
                m_offlinemsgmoodel.remove(user.getId());
            }

            // 查询好友信息
            std::vector<User> friends = m_friendmodel.query(user.getId());
            if(!friends.empty()){
                std::vector<std::string> vec;
                for(auto& user: friends){
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec.emplace_back(js.dump());
                }
                response["friends"] = vec;
            }
            
            std::vector<Group> groups = m_groupmodel.queryGroups(user.getId());
            if(!groups.empty()){
                std::vector<std::string> vec;
                for(auto& group: groups){
                    json js;
                    js["id"] = group.getId();
                    js["gname"] = group.getName();
                    js["gdesc"] = group.getDesc();
                    std::vector<std::string> vec1;
                    for(auto& user: group.getUsers()){
                        json js1;
                        js1["id"] = user.getId();
                        js1["name"] = user.getName();
                        js1["state"] = user.getState();
                        js1["role"] = user.getRole();
                        vec1.emplace_back(js1.dump());
                    }
                    js["users"] = vec1;
                    vec.emplace_back(js.dump());
                }
                response["groups"] = vec;
            }

            if(conn->connected()){
                m_usermodel.updateState(user);  
                conn->send(response.dump());
            }
        }

    }else{
        response["msgid"] = getEnumValue(EnMsgType::MSG_LOGIN_ACK);
        response["errno"] = 1;
        response["errinfo"] = "invalid account or password";
        conn->send(response.dump());
    }
}


void ChatService::reg(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp){
    User user;
    user.setName(js["name"]);
    user.setPwd(js["pwd"]);

    bool res = m_usermodel.insert(user);
    json response;  
    if (res){
        response["msgid"] = getEnumValue(EnMsgType::MSG_REG_ACK);
        response["errno"] = 0;
        response["id"] = user.getId();
    }else{
        response["msgid"] = getEnumValue(EnMsgType::MSG_REG_ACK);
        response["errno"] = 1;
    }
    conn->send(response.dump());
}

void ChatService::clientCloseException(const muduo::net::TcpConnectionPtr& conn){
    User user;    

    {   // 删除用户连接信息
        std::lock_guard<std::mutex> l(m_conMutex);
        for(const auto& [key, connection]: m_userConMap){
            if(connection == conn){
                m_userConMap.erase(key);
                user.setId(key);
                break;
            }
        }
    }
    // 设置用户为offline
    user.setState("offline");
    m_usermodel.updateState(user);
}

void ChatService::onechat(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp){
    int toId = js["to"].get<int>();
    bool isOnline = true;

    {
        std::lock_guard<std::mutex> l(m_conMutex);
        auto it = m_userConMap.find(toId);
        isOnline = (it != m_userConMap.end());
        if(isOnline && it->second->connected()){ // 这里需要保存离线信息所以还是要判断一下连接是否还在
            // 在线, 服务器直接把消息传给目标用户
            it->second->send(js.dump());
            return;
        }
    }
    // 离线，写入离线表中
    m_offlinemsgmoodel.insert(toId, js.dump());
}

void ChatService::reset(){
    // 将所有online状态的用户重置为offline
    m_usermodel.resetState();
}

void ChatService::addfriend(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp){
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 保存好友信息
    m_friendmodel.insert(userid, friendid);
}

void ChatService::creategroup(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp){
    int userid = js["id"].get<int>();
    std::string name = js["gname"];
    std::string desc = js["gdesc"];
    Group group;
    group.setName(name);
    group.setDesc(desc);

    if(m_groupmodel.createGroup(group)){
        m_groupmodel.addGroup(userid, group.getId(), "creator");
    }

}


void ChatService::joingroup(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp){
    int userid = js["id"].get<int>();
    int groupid = js["gid"].get<int>();
    m_groupmodel.addGroup(userid, groupid, "normal");
}


void ChatService::groupchat(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp){
    int userid = js["id"].get<int>();
    int groupid = js["gid"].get<int>();
    std::vector<int> members = m_groupmodel.queryMembers(userid, groupid);

    std::lock_guard<std::mutex> l(m_conMutex);
    for(auto& member: members){
        auto it = m_userConMap.find(member);
            if(it != m_userConMap.end()){
                it->second->send(js.dump());
            }else{
                m_offlinemsgmoodel.insert(member, js.dump());
            }
    }
}

void ChatService::logout(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp){
    int userid = js["id"];
    User user;
    user.setId(userid);
    user.setState("offline");
    m_usermodel.updateState(user);
    {   // 删除用户连接信息
        std::lock_guard<std::mutex> l(m_conMutex);
        for(const auto& [key, connection]: m_userConMap){
            if(key == userid){
                m_userConMap.erase(key);
                break;
            }
        }
    }
}