#include<iostream>
#include "chat_server.h"
ChatServer::ChatServer()
{
    //初始化事件集合
    this->base=event_base_new();
    //初始化数据库对象
    db=new DataBase();
    //初始化数据库user表(chat_user,chat_group)
    if(!db->database_init_table())
    {
        std::cout<<"init table failure"<<std::endl;
        exit(1);
    }
    //初始化数据结构对象
    info=new ChatInfo();
    //初始化群消息:把群消息从数据库读出来，放入map
   server_updata_group_info();
   //初始化线程池
   thread_number=3;
   cur_thread=0;
   pool = new ChatThread[thread_number];
    for(int i = 0;i<thread_number;i++)
    {
        pool[i].start(info,db);
    }
        
}
void ChatServer:: server_updata_group_info()
{   
    if(!db->database_connent())
    {
        exit(1);
    }
    std::string groupinfo[1024];  //群最多1024个    
    int num= db->database_get_group_info(groupinfo);
    std::cout<<"---group number "<<num<<std::endl; 
    db->database_disconnect();
    info->list_update_group(groupinfo,num);
    //info->list_print_group();

}
ChatServer::~ChatServer()
{
    if(db)
    {
        delete db;
    }
    if(info)
    {
        delete info;
    }
}
void ChatServer::listen(const char * ip,int port)
{
    struct sockaddr_in server_info;
    memset(&server_info,0,sizeof(server_info));
    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr=inet_addr(ip);
    server_info.sin_port = htons(port);
    
    struct evconnlistener*listener= evconnlistener_new_bind(base,listener_cb,this,LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,5,(struct sockaddr*)&server_info,sizeof(server_info));
    if(NULL==listener)
    {
       std::cout<<"evconnlistener_new_bind error"<<std::endl;
       exit(1);
    }
    //监听集合
    event_base_dispatch(base);  //死循环
    //释放对象
    evconnlistener_free(listener);
    event_base_free(base);

}
//回调函数,有客户端发起连接会触发该函数
void ChatServer:: listener_cb(struct evconnlistener *listner,evutil_socket_t fd,struct sockaddr * c,int socklen,void *arg)
{   
    ChatServer *ser=(ChatServer *)arg;
    struct sockaddr_in * client_info=(struct sockaddr_in *)c;
    std::cout<<"[connection]";
    std::cout<<"client ip :"<<inet_ntoa(client_info->sin_addr);
    std::cout<<" port : "<<client_info->sin_port<<std::endl;
    //创建事件,放入线程池

    ser->server_alloc_event(fd);
}
void ChatServer::server_alloc_event(int fd)
{
    struct event_base*t_base = pool[cur_thread].thread_get_base();
  
    struct bufferevent *bev = bufferevent_socket_new(t_base,fd,BEV_OPT_CLOSE_ON_FREE);
    if(NULL==bev)
    {
        std::cout<<"bufferevent_socket_new error"<<std::endl;
        return ;
    }
    bufferevent_setcb(bev,ChatThread::thread_readcb,NULL,ChatThread::thread_eventcb,&pool[cur_thread]);
    bufferevent_enable(bev,EV_READ);
    std::cout<<" to thread "<<pool[cur_thread].thread_get_id()<<std::endl;
      cur_thread = (cur_thread+1)%thread_number;
}