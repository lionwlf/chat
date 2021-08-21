#ifndef USER_H_
#define USER_H_


#include<string>
using namespace std;

//User表的映射类
class User{
private:
    int id;
    string name;
    string password;
    string state;

public:
    User(int id = -1,string name = "",string pwd = "",string state = "offline"){
        this->id = id;
        this->name = name;
        this->password = pwd;
        this->state = state;
    }

    void setID(int id){
        this->id = id;
    }

    void setname(string name){
        this->name = name;
    }

    void setpassword(string password){
        this->password = password;
    }

    void setstate(string state){
        this->state = state;
    }

    int getID(){
        return this->id;
    }

    string getname(){
         return this->name;
    }

    string getpassword(){
        return this->password;
    }

    string getstate(){
        return this->state;
    }
};

#endif