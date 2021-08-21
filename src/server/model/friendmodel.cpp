#include"friendmodel.hpp"
#include"db.hpp"

void FriendModel::insert(int userid,int friendid){
    char sql[128] = {0};

    sprintf(sql,"insert into Friend(userid,friendid) values(%d,%d)",userid,friendid);

    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}

vector<User> FriendModel::query(int userid){
    // 1.组装sql语句
    char sql[256] = {0};
    sprintf(sql, "select a.id,a.name,a.state from User a inner join Friend b on b.friendid = a.id where b.userid = %d", userid);

    vector<User> vec;

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                User user;
                user.setID(atoi(row[0]));
                user.setname(row[1]);
                user.setstate(row[2]);

                vec.push_back(user);
            }
            mysql_free_result(res);
        }
    }

    return vec;
}