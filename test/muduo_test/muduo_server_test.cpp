/*
    muduo网络库提供：
    TcpServer：用于编写服务器程序
    TcpClient：用于编写客户端程序

    epoll+线程池

    将网络I/O与业务代码区分开，让程序员可以专心开发业务代码
*/

#include<iostream>
#include<muduo/net/EventLoop.h>
#include<muduo/net/TcpServer.h>
#include<string>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;


class ChatServer{
/*
    1、组合TcpServer对象
    2、创建EventLoop事件循环的指针
    3、明确TcpServer构造函数需要的参数
    4、在类的构造函数中绑定用户注册和断开、用户消息读写的回调
    5、设置线程数，muduo库会自行分配I/O线程和任务线程
*/

public:
    ChatServer(EventLoop* loop, //事件循环
            const InetAddress& listenAddr,  //IP+Port
            const string& nameArg)  //服务器名字
            :_server(loop,listenAddr,nameArg),_loop(loop)
    {
        //使用绑定器，给服务器注册用户创建会断开回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));        

        //给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));

        //设置线程数量
        _server.setThreadNum(4);
    }

    //开启事件循环
    void start(){
        _server.start();
    }

private:
    
    //用户连接的创建和断开回调
    void onConnection(const TcpConnectionPtr& cb){
        if(cb->connected()){
            cout<<cb->peerAddress().toIpPort()<<"："<<cb->localAddress().toIpPort()<<"上线"<<endl;
        }
        else{
            cout<<cb->peerAddress().toIpPort()<<"："<<cb->localAddress().toIpPort()<<"下线"<<endl;
            cb->shutdown();
        }
    }

    //处理读写事件的回调函数
    void onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp time){
        string buff = buf->retrieveAllAsString();
        cout<<"recv date："<<buff<<time.toString()<<endl;

        conn->send(buff);
    }

    TcpServer _server;  // #1
    EventLoop *_loop;   // #2 epoll

};


int main(){

    EventLoop loop;

    InetAddress addr("127.0.0.1",6000);

    ChatServer server(&loop,addr,"ChatServer");

    server.start();

    loop.loop();    //epoll_wait

    return 0;
}