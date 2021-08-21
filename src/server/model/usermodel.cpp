#include "usermodel.hpp"
#include "db.hpp"
#include<iostream>

using namespace std;

bool UserModel::insert(User &user){
    char sql[128] = {0};

    sprintf(sql,"insert into User(name,password,state) values('%s','%s','%s')",
        user.getname().c_str(),user.getpassword().c_str(),user.getstate().c_str());

    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            user.setID(mysql_insert_id(mysql.getconnection()));
            return true;
        }
    }
    return false;
}

// 根据用户号码查询用户信息
User UserModel::query(int id)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from User where id = %d", id);

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setID(atoi(row[0]));
                user.setname(row[1]);
                user.setpassword(row[2]);
                user.setstate(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }

    return User();
}

bool UserModel::updateState(User user){
    char sql[128] = {0};

    sprintf(sql,"update User set state = '%s' where id = '%d'",user.getstate().c_str(),user.getID());

    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}

void UserModel::resetstate()
{
    // 1.组装sql语句
    char sql[1024] = "update User set state = 'offline' where state = 'online'";

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}