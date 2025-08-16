#include "redis.hpp"

#include <thread>

#include <muduo/base/Logging.h>

Redis::Redis():m_publish(nullptr), m_subscribe(nullptr){}

Redis::~Redis(){
    if(m_publish != nullptr){
        redisFree(m_publish);
    }
    if(m_subscribe != nullptr){
        redisFree(m_subscribe);
    }
}

bool Redis::connect(){
    m_publish = redisConnect("127.0.0.1", 6379);
    if(m_publish == nullptr){
        LOG_ERROR<< "publish context fail";
        return false;
    }

    m_subscribe = redisConnect("127.0.0.1", 6379);
    if(m_subscribe == nullptr){
        LOG_ERROR<< "publish context fail";
        return false;
    }


    // 创建独立线程监听订阅事件
    std::thread t([this]{
        oberver_channel_message();
    });
    t.detach();
    return true;
}

bool Redis::publish(int channel, const std::string& message){
    redisReply* reply = (redisReply *)redisCommand(m_publish, "PUBLISH %d %s", channel, message.c_str());
    if(reply == nullptr){
        LOG_ERROR<< "publish fail";
        return false;
    }
    freeReplyObject(reply);
    return true;
}

bool Redis::subscribe(int channel){
    // 把命令组装到本地
    if(REDIS_ERR == redisAppendCommand(m_subscribe, "SUBSCRIBE %d", channel)){
        LOG_ERROR << "subscribe fail";
        return false;
    }
    // 把命令发送到redis server
    int done = 0;
    while(!done){
        if(REDIS_ERR == redisBufferWrite(m_subscribe, &done)){
            LOG_ERROR << "subscribe fail";
            return false;
        }
    }
    // 不阻塞等待执行完成
    return true;
}

bool Redis::unsubscribe(int channel){
    if(REDIS_ERR == redisAppendCommand(m_subscribe, "UNSUBSCRIBE %d", channel)){
    LOG_ERROR << "unsubscribe fail";
        return false;
    }
    return true;
}

void Redis::oberver_channel_message(){
    redisReply* reply = nullptr;
    while(REDIS_OK == redisGetReply(m_subscribe, (void**)&reply)){
        if(reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr){
            m_cb(atoi(reply->element[1]->str), reply->element[2]->str);
        }
        freeReplyObject(reply);
    }
}

void Redis::setCb(CbFunc func){
    m_cb = func;
}