#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H
#define IP "10.0.20.16"
#include"chat_database.h"
#include"chat_list.h"
#define PORT 8888
#include<event.h>
#include<stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <json/json.h>
#include <list>
#include <unistd.h>
#include <time.h>
#include"chat_thread.h"
class ChatServer
{
private:
    struct event_base * base; //监听事件集合
    DataBase *db;                   //数据库对象
    ChatInfo *info;    
    ChatThread *pool;
    int thread_number;
    int cur_thread;
public:
    ChatServer();
    ~ChatServer();
    void listen(const char *ip,int port);
    static void listener_cb(struct evconnlistener *listner,evutil_socket_t fd,struct sockaddr * client_info,int socklen,void *);
    void server_updata_group_info();
    void server_alloc_event(int );

};
#endif