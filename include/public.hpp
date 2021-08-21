#ifndef PUBLIC_H_
#define PUBLIC_H_

enum EnMsgType{
    LOGIN_TYPE = 1, //正常登录
    LOGIN_MSG_ACK,  //登录相应消息
    REG_TYPE,       //正常注册
    REG_MSG_ACK,    //注册相应消息
    ONE_CHAT_MSG,   //一对一聊天消息
    ADD_FRINEND_MSG,//添加好友
    CREATE_GROUP_MSG, // 创建群组
    ADD_GROUP_MSG, // 加入群组
    GROUP_CHAT_MSG, // 群聊天
};

#endif