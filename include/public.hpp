#ifndef _PUBLIC_
#define _PUBLIC_

#include <type_traits>
#include <memory>
#include <mutex>

enum class EnMsgType{
    MSG_REG,
    MSG_REG_ACK, // 表示注册回复
    MSG_LOGIN,
    MSG_LOGIN_ACK, // 表示登录回复
    MSG_ONE_CHAT,  // 一对一聊天消息
    MSG_ADD_FRIEND, // 添加好友信息
    MSG_CREATE_GROUP,//创建组
    MSG_JOIN_GROUP, // 加入组
    MSG_GROUP_CHAT,  // 组聊天
    MSG_LOGOUT, //注销
    MSG_GET_FRIENDS, // 拉取所有好友状态
    MSG_GET_FRIENDS_REP, // 拉取好友的返回信息
    MSG_GET_GROUP, // 拉取所有群组
    MSG_GET_GROUP_REP, // 拉取群组的返回信息
    MSG_ONLINE_NOTIFY, // 服务器给客户端的上线通知
    MSG_OFFLINE_NOTIFY, //服务器给客户端的下线通知
};

using MsgUnderType = std::underlying_type_t<EnMsgType>;

inline MsgUnderType getEnumValue(EnMsgType e){
    return static_cast<MsgUnderType>(e);
}

#define GetEnumString(x) #x

template<typename T>
class Singleton{
public:
    template<typename ...Args>
    static std::shared_ptr<T> instance(Args&&... args){
        std::call_once(m_onceflag, [...args = std::forward<Args>(args)]{
            m_ptr.reset(new T(std::forward<Args>(args)...));
        });
        return m_ptr;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

private:
    static std::shared_ptr<T> m_ptr;
    static std::once_flag m_onceflag;
};

template<typename T>
std::once_flag Singleton<T>::m_onceflag;

template<typename T>
std::shared_ptr<T> Singleton<T>::m_ptr = nullptr;


#endif