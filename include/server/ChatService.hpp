#ifndef _CHAT_SERVICE_
#define _CHAT_SERVICE_

#include <unordered_map>
#include <functional>
#include <mutex>

#include <muduo/net/TcpServer.h>
#include "json.hpp"
using json = nlohmann::json;

#include "public.hpp"
#include "UserModel.hpp"
#include "OfflineMsgModel.hpp"
#include "FriendModel.hpp"
#include "GroupModel.hpp"
#include "redis.hpp"

using CbType = std::function<void(const muduo::net::TcpConnectionPtr&, json&, muduo::Timestamp)>;

class ChatService : public Singleton<ChatService> {
    friend class Singleton<ChatService>;
public:
    template<typename Obj>
    void registeHandler(EnMsgType msgtype, auto&& handler);

    template <typename Obj>
    void registeHandler(EnMsgType msgtype, Obj* obj, void (Obj::*method)(const muduo::net::TcpConnectionPtr&, json&, muduo::Timestamp));

    CbType getHandler(MsgUnderType id);
    CbType getHandler(EnMsgType msgtype);

    void login(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp);
    void reg(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp);
    void onechat(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp);
    void addfriend(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp);
    void creategroup(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp);
    void joingroup(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp);
    void groupchat(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp);
    void logout(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp);
    void getfriends(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp);
    void getgroups(const muduo::net::TcpConnectionPtr& conn, json& js, muduo::Timestamp tsp);

    void getfriendshandler(json& js, int userid);
    void getgroupshandler(json& js, int userid);
    void sendToOnlineFriends(int userid, EnMsgType msgtype);

    void redisMessageHandler(int channel, const std::string& message);

    // 处理客户端异常退出
    void clientCloseException(const muduo::net::TcpConnectionPtr& conn);
    // 处理服务器异常退出
    void reset();

    ~ChatService() = default;
private:

    ChatService();
    
    std::unordered_map<MsgUnderType, CbType> m_cbFuncMap;

    std::mutex m_conMutex; // 给下面的映射关系加锁
    std::unordered_map<int, muduo::net::TcpConnectionPtr> m_userConMap;

    UserModel m_usermodel;
    OfflineMsgModel m_offlinemsgmoodel;
    FriendModel m_friendmodel;
    GroupModel m_groupmodel;

    Redis m_redis;

};



#endif