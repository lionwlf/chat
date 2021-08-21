#ifndef CHATSERVICE_H_
#define CHATSERVICE_H_

#include<muduo/net/TcpConnection.h>
#include<unordered_map>
#include<functional>
#include<mutex>

#include "json.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"

using json = nlohmann::json;
using namespace std;
using namespace muduo;
using namespace muduo::net;


//处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn,json &js,Timestamp time)>;

//聊天服务器业务
class ChatService{
public:
    //单例模式
    static ChatService* instance();
    
    void login(const TcpConnectionPtr &conn,json &js,Timestamp time);
    void reg(const TcpConnectionPtr &conn,json &js,Timestamp time);

    //一对一聊天消息
    void onechat(const TcpConnectionPtr &conn,json &js,Timestamp time);
    
    // 创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    
    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    
    // 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //获取消息对应的处理器
    MsgHandler getHandle(int msgid);

    //添加好友
    void addFriend(const TcpConnectionPtr &conn,json &js,Timestamp time);

    // 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);

    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    //处理服务端异常退出
    void reset();
private:

    ChatService();

    //存储消息id和对应的处理方法
    unordered_map<int,MsgHandler> _msgHanderMap;

    //存储在线用户连接
    unordered_map<int,TcpConnectionPtr> _userConnMap;
    
    //数据操作类的对象
    UserModel _usermodel;
    OfflineMessageModel _offlineMsgmodel;
    FriendModel _friendmodel;
    GroupModel _groupModel;

    //定义互斥锁
    mutex _connMutex;

    // redis操作对象
    Redis _redis;
};


#endif