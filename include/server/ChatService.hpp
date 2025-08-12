#ifndef _CHAT_SERVICE_
#define _CHAT_SERVICE_

#include <unordered_map>
#include <functional>

#include <muduo/net/TcpServer.h>
#include "json.hpp"
using json = nlohmann::json;

#include "public.hpp"
#include "UserModel.hpp"

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

    ~ChatService() = default;
private:

    ChatService();
    
    std::unordered_map<MsgUnderType, CbType> m_cbFuncMap;

    UserModel m_usermodel;
};



#endif