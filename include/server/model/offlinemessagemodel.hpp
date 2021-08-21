#ifndef OFFLINEMESSAGEMODEL_H_
#define OFFLINEMESSAGEMODEL_H_

#include<string>
#include<vector>

using namespace std;


class OfflineMessageModel{
public:
    //存储用户离线消息
    void insert(int user_id,string msg);

    //删除用户离线消息
    void remove(int userid);

    //读取用户离线消息
    vector<string> query(int userid);
    
private:
    
};

#endif