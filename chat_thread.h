#ifndef CHAT_THREAD_H
#define CHAT_THREAD_H
#include<thread>
#include<event.h>
#include "chat_list.h"
#include "chat_database.h"
#include <cstring>
#include <json/json.h>
#include <unistd.h>
class ChatThread
{
private:
    std::thread *_thread;
    std::thread::id _id;
    struct event_base *base;
    ChatInfo *info;
    DataBase *db;
public:
    ChatThread();
    ~ChatThread();
    void run();
    std::thread::id thread_get_id();
    static void worker(ChatThread *) ;
    void start(ChatInfo *,DataBase*);
    static void timeout_cb(evutil_socket_t fd,short event,void*arg);
    struct event_base *thread_get_base();
    static void thread_readcb(struct bufferevent *,void*);
    static void thread_eventcb(struct bufferevent *,short ,void* );
    bool thread_read_data(struct bufferevent *,char *);
    void thread_registr(struct bufferevent *,Json::Value &);
    void  thread_write_data(struct bufferevent *,Json::Value &);
    void thread_login(struct bufferevent *,Json::Value &);
    bool database_password_correct(Json::Value &);
    void thread_add_friend(struct bufferevent *,Json::Value &);
    int thread_parse_string(std::string &,std:: string *);
    void thread_private_chat(struct bufferevent *,Json::Value &);
    void thread_create_group(struct bufferevent *,Json::Value &);
    void thread_join_group(struct bufferevent *,Json::Value &);
    void thread_group_chat(struct bufferevent *,Json::Value &);
    void thread_transfer_file(struct bufferevent *,Json::Value &);
    void thread_client_offline(struct bufferevent *,Json::Value &);
};
#endif