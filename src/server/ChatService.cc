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
    registeHandler<ChatService>(EnMsgType::MSG_ONE_CHAT, this, &ChatService::onechat);
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
        }else{
            user.setState("online");
            m_usermodel.updateState(user);
            

            {
                std::lock_guard<std::mutex> l(m_conMutex);
                m_userConMap.insert_or_assign(user.getId(), conn);
            }

            response["msgid"] = getEnumValue(EnMsgType::MSG_LOGIN_ACK);
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
        }

    }else{
        response["msgid"] = getEnumValue(EnMsgType::MSG_LOGIN_ACK);
        response["errno"] = 1;
        response["errinfo"] = "invalid account or password";
    }

    conn->send(response.dump());
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
        if(isOnline){
            // 在线, 服务器直接把消息传给目标用户
            it->second->send(js.dump());
            return;
        }
    }
    // 离线，写入离线表中

}