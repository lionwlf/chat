#include"chatservice.hpp"
#include"public.hpp"
#include<string>
#include<vector>
#include<map>
#include<muduo/base/Logging.h>

//#include<iostream>

using namespace std;
using namespace muduo;

ChatService* ChatService::instance(){
    static ChatService service;

    return &service;
}
    
//注册消息以及对应的回调操作
ChatService::ChatService(){
    _msgHanderMap.insert({LOGIN_TYPE,std::bind(&ChatService::login,this,_1,_2,_3)});
    _msgHanderMap.insert({REG_TYPE,std::bind(&ChatService::reg,this,_1,_2,_3)});
    _msgHanderMap.insert({ONE_CHAT_MSG,std::bind(&ChatService::onechat,this,_1,_2,_3)});
    _msgHanderMap.insert({ADD_FRINEND_MSG,std::bind(&ChatService::addFriend,this,_1,_2,_3)});
 
    // 群组业务管理相关事件处理回调注册
    _msgHanderMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHanderMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHanderMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});

    // 连接redis服务器
    if (_redis.connect()){
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

//获取存储消息id和对应的处理方法
MsgHandler ChatService::getHandle(int msgid){

    //日志记录
    auto it = _msgHanderMap.find(msgid);
    if(it == _msgHanderMap.end()){
        //返回一个lambda表达式，返回一个默认的空处理器，防止业务挂掉，后可做平滑升级处理        
        return [=](const TcpConnectionPtr &conn,json &js,Timestamp time){
            LOG_ERROR<<"msgid:"<<msgid<<"can not find handle!";
        };
    }
    else{
        return _msgHanderMap[msgid];
    }
}

void ChatService::login(const TcpConnectionPtr &conn,json &js,Timestamp time){
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _usermodel.query(id);
    if (user.getID() == id && user.getpassword() == pwd)
    {
        if (user.getstate() == "online")
        {
            // 该用户已经登录，不允许重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using, input another!";

            conn->send(response.dump());
        }
        else
        {   
            //添加作用域，限制锁的粒度
            {
                lock_guard<mutex> lock(_connMutex);
          
                //记录用户连接
                _userConnMap.insert({id,conn});
            }
            
            // id用户登录成功后，向redis订阅channel(id)
            _redis.subscribe(id); 

            // 登录成功，更新用户状态信息 state offline=>online
            user.setstate("online");
            _usermodel.updateState(user);       //    !!!     Single stepping until exit from function _IO_default_xsputn,

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getID();
            response["name"] = user.getname();
            
            //查询用户是否有离线消息
            vector<string> vecofflinemsg = _offlineMsgmodel.query(id);
            if(!vecofflinemsg.empty()){
                response["offlinemsg"] = vecofflinemsg;

                //清空离线消息
                _offlineMsgmodel.remove(id);
            }

            vector<User> uservec = _friendmodel.query(id);
            if(!uservec.empty()){
                vector<string> vecfriend;
                for(User &user:uservec){
                    json js;
                    js["id"] = user.getID();
                    js["name"] = user.getname();
                    js["state"] = user.getstate();

                    vecfriend.push_back(js.dump());
                }
                response["friends"] = vecfriend;
            }

            conn->send(response.dump());
        }
    }
    else
    {
        // 该用户不存在，用户存在但是密码错误，登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "id or password is invalid!";
        conn->send(response.dump());
    }
}

void ChatService::reg(const TcpConnectionPtr &conn,json &js,Timestamp time){
    string name = js["name"];
    string pwd = js["password"];
    
    User _user;
    _user.setname(name);
    _user.setpassword(pwd);

    bool state = _usermodel.insert(_user);

    if(state){
        //注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = _user.getID();

        conn->send(response.dump());
    }
    else{
        //注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
       
        conn->send(response.dump());
    }
}

// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid); 

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid); 

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    _usermodel.updateState(user);
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn){
    User user;

    {
        //以conn从哈希表中倒查主键id
        lock_guard<mutex> lock(_connMutex);
        
        for(auto it = _userConnMap.begin();it!=_userConnMap.end();){
            if(it->second == conn){
                //更改用户状态
                user.setID(it->first);

                //从hash表删除用户信息
                _userConnMap.erase(it);
                break; 
            }
            else{
                it++;
            }
        }
    }
     
    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(user.getID()); 

    //数据持久化
    if(user.getID() != -1){
        user.setstate("offline");   //再细一点
        _usermodel.updateState(user);
    }
}

//一对一聊天
void ChatService::onechat(const TcpConnectionPtr &conn,json &js,Timestamp time){
//    cout<<js<<endl;

    int toid = js["toid"].get<int>(); //这里bug

    // bool userstate = false;
    //开辟锁的作用域
    {
        lock_guard<mutex> lock(_connMutex);

        auto it = _userConnMap.find(toid);

        if(it != _userConnMap.end()){  
            //用户在线，转发消息
            it->second->send(js.dump());
            return;
        }   
    }

    // 查询toid是否在线 
    User user = _usermodel.query(toid);
    if (user.getstate() == "online"){
        _redis.publish(toid, js.dump());
        return;
    }

    // toid不在线，存储离线消息
    _offlineMsgmodel.insert(toid, js.dump());
}
    
//添加好友
void ChatService::addFriend(const TcpConnectionPtr &conn,json &js,Timestamp time){
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    _friendmodel.insert(userid,friendid);
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group)){
        // 存储群组创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}

// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(_connMutex);
    for (int id : useridVec){
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end()){
            // 转发群消息
            it->second->send(js.dump());
        }
        else{
            User user = _usermodel.query(id);
            if (user.getstate() == "online"){
                _redis.publish(id, js.dump());
            }
            else{
                // 存储离线群消息
                _offlineMsgmodel.insert(id, js.dump());
            }
        }
    }
}


// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg){
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgmodel.insert(userid, msg);
}


void ChatService::reset(){
    //把所有online状态的用户转为offline
    _usermodel.resetstate();
}


