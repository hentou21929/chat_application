#ifndef CHAT_DATABASE_H
#define CHAT_DATABASE_H
#include <mysql/mysql.h>
#include <mutex>
#include<iostream>
#include<stdio.h>
#include <json/json.h>
#include<cstring>
class DataBase{
private:
    MYSQL *mysql;
    std::mutex _mutex;
public:
    DataBase();
    ~DataBase();
    bool database_connent();
    void database_disconnect();
    int database_get_group_info(std::string *);
    bool database_init_table();
    bool database_user_is_exist(std::string );
    void database_insert_user_info(Json::Value &);
    bool  database_password_correct(Json::Value &val);
    bool database_get_friend_group(Json::Value &v,std::string &,std::string &);
    void database_add_friend(Json::Value &);
    void database_up_friendlist(std::string &u,std::string &f);
    void add_new_group(std::string,std::string);
    void database_updata_group_member(std::string,std::string);
    
};
#endif