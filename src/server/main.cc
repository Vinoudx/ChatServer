#include "ChatServer.hpp"
#include "ChatService.hpp"

#include <iostream>
#include <signal.h>

#include <ChatService.hpp>

using namespace std;

void sigint_handler(int){
    ChatService::instance()->reset();
    exit(0);
}

int main(){

    signal(SIGINT, sigint_handler);

    muduo::net::EventLoop eventloop;
    muduo::net::InetAddress addr("127.0.0.1", 6666);
    ChatServer s(&eventloop, addr, "chat");

    s.start();
    eventloop.loop();
}