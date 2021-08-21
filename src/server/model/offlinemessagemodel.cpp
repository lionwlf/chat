#include "db.hpp"
#include "offlinemessagemodel.hpp"

//存储用户离线消息
void OfflineMessageModel::insert(int userid, string msg){
    char sql[128] = {0};

    sprintf(sql, "insert into offlinemessage values(%d, '%s')", userid, msg.c_str());
    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}

//删除用户离线消息
void OfflineMessageModel::remove(int userid){
    char sql[128] = {0};

    sprintf(sql,"delete from OfflineMessage where userid = %d",userid);

    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}

//读取用户离线消息
vector<string> OfflineMessageModel::query(int userid){
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from OfflineMessage where userid = %d", userid);

    vector<string> vec;

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        
        if (res != nullptr)
        {

            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}

