#include "ChatServer.hpp"
#include <iostream>
#include <string>

#include "ChatService.hpp"

ChatServer::ChatServer(muduo::net::EventLoop* loop, 
                        const muduo::net::InetAddress& listenAddr, 
                        const std::string& nameArg):m_eventloop_ptr(loop), m_server(loop, listenAddr, nameArg){
    m_server.setConnectionCallback([this](const muduo::net::TcpConnectionPtr& p)->void{
        this->onConnection(p);
    });

    m_server.setMessageCallback([this](const muduo::net::TcpConnectionPtr& p, muduo::net::Buffer* buf, muduo::Timestamp tsp)->void{
        this->onMessage(p, buf, tsp);
    });

    m_server.setThreadNum(std::thread::hardware_concurrency());

}

void ChatServer::start(){
    m_server.start();
}

void ChatServer::onConnection(const muduo::net::TcpConnectionPtr& conn){
    if(conn->disconnected()){
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp tsp){
    std::string message = buf->retrieveAllAsString();
    json j = json::parse(message);
    auto handler = ChatService::instance()->getHandler(j["msgid"].get<MsgUnderType>());
    handler(conn, j, tsp);
}