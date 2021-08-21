#include "chatserver.hpp"
#include <functional>
#include <string>
#include "chatservice.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
using namespace placeholders;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg) : _server(loop, listenAddr, nameArg),
                                                _loop(loop)
{
    //注册连接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    //注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    //设置线程数
    _server.setThreadNum(5);
}

void ChatServer::start()
{
    _server.start();
}


void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buff, Timestamp time){
    string buf = buff->retrieveAllAsString();

    json js = json::parse(buf);

    //通过msgid获取业务回调，进行网络模块和任务模块之间的解耦合
    auto msgHandler = ChatService::instance()->getHandle(js["msgid"].get<int>());
    
    //回调消息绑定好的事件处理器，执行相应的业务处理
    msgHandler(conn,js,time);

    //成功解耦
}

void ChatServer::onConnection(const TcpConnectionPtr &conn){
    if(!conn->connected()){ //用户断开连接

        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

