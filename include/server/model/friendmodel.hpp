#ifndef FRIENDMODEL_H_
#define FRIENDMODEL_H_

#include<vector>
#include"user.hpp"

using namespace std;

//好友信息方法
class FriendModel{
public:
    void insert(int userid,int friendid);

    vector<User> query(int userid);

private:

};

#endif