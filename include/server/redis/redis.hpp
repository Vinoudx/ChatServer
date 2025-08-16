#ifndef _REDIS_
#define _REDIS_

#include <string>
#include <functional>

#include <hiredis/hiredis.h>

class Redis{
public:
    Redis();
    ~Redis();

    using CbFunc = std::function<void(int, const std::string&)>;

    bool connect();
    bool publish(int channel, const std::string& message);
    bool subscribe(int channel);
    bool unsubscribe(int channel);
    // 在独立线程中接受订阅通道中的消息
    void oberver_channel_message();
    void setCb(CbFunc fn);

private:
    redisContext* m_publish;
    redisContext* m_subscribe;
    CbFunc m_cb;
};

#endif