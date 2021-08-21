#ifndef USERMODEL_H_
#define USERMODEL_H_

#include "user.hpp"

//User表的操作类
class UserModel{
public:
    bool insert(User &user);
    User query(int id);
    bool updateState(User user);
    void resetstate();

private:

};

#endif