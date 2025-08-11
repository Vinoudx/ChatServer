#include "ChatServer.hpp"
#include "ChatService.hpp"

#include <iostream>
using namespace std;

int main(){
    muduo::net::EventLoop eventloop;
    muduo::net::InetAddress addr("127.0.0.1", 6666);
    ChatServer s(&eventloop, addr, "chat");

    s.start();
    eventloop.loop();
}