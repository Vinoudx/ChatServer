#include "ChatService.hpp"

#include <muduo/base/Logging.h>

#include "User.hpp"

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
    LOG_INFO << "someone login !";
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
        response["userid"] = user.getId();
    }else{
        response["msgid"] = getEnumValue(EnMsgType::MSG_REG_ACK);
        response["errno"] = 1;
    }
    conn->send(response.dump());
}