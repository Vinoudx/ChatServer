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

int main(int args, char** argv){

    signal(SIGINT, sigint_handler);

    if(args < 3){
        std::cout<< "not enough params\n";
        return -1;
    }

    muduo::net::EventLoop eventloop;
    muduo::net::InetAddress addr(argv[1], atoi(argv[2]));
    ChatServer s(&eventloop, addr, "chat");
    
    s.start();
    eventloop.loop();
}