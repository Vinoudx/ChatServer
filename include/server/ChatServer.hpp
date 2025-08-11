#ifndef _CHAT_SERVER_
#define _CHAT_SERVER_

#include <string>
#include <thread>

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

class ChatServer{
public:
    ChatServer(muduo::net::EventLoop* loop,
            const muduo::net::InetAddress& listenAddr,
            const std::string& nameArg);
    
    void start();

private:

    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp tsp);

    muduo::net::TcpServer m_server;
    muduo::net::EventLoop* m_eventloop_ptr;
};


#endif