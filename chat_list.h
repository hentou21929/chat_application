#ifndef CHAT_LIST_H
#define CHAT_LIST_H
#include<iostream>
#include<list>
#include <mutex>
#include<map>
#include<thread>
#include<event.h>
#include "chat_list.h"
#include "chat_database.h"
#include <cstring>
#include <json/json.h>

struct User
{
    std::string name;                       //  用户名
    struct bufferevent *bev;                //  客户端对应的事件
};
class ChatInfo
{
private:
    //保存存在用户信息
    std::list<User>*online_user;
    //保存所有群信息
    std::map<std::string, std::list<std::string>> *group_info;
    //访问在线用户的锁
    std::mutex List_mutex;
    //访问群消息的锁
    std::mutex map_mutex;
public:
    ChatInfo();
    ~ChatInfo();
    void list_update_group(std::string *,int );
    void list_print_group();
    bool list_updata_list(Json::Value &,struct bufferevent *);
    struct bufferevent * list_friend_online(std::string);
    bool list_group_is_exist(std::string);
    void list_add_new_group(std::string,std::string);
    bool list_member_is_group(std::string,std::string);
    void list_update_group_member(std::string,std::string);
    std::list<std::string> &list_get_list(std::string);
    void list_delete_user(std::string);
};
#endif