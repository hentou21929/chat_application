#include"chat_list.h"
ChatInfo::ChatInfo()
{
    online_user = new std:: list<User>;
    group_info = new std::map<std::string, std::list<std::string>>;
}
ChatInfo::~ChatInfo()
{
    if(online_user)
    {
        delete online_user;
    }
    if(group_info)
    {
        delete group_info;
    }
}
void ChatInfo::list_update_group(std::string *g,int size)
{   
    int idx,start;
    start=0;
    std:: string groupname,membername;
    std::list<std::string> l;
    for(int i=0;i<size;i++)
    {

        idx = g[i].find("|");
        start=idx+1;
        groupname = g[i].substr(0,idx);
        //std::cout<<groupname<<std::endl;
        while(1)
        {   
            
            idx = g[i].find('|',idx+1);
            if(idx==-1)
            {
                break;
            }
            membername = g[i].substr(start,idx-start);
            l.push_back(membername);
            start =idx++;
        }
        membername = g[i].substr(start,idx-start);
        l.push_back(membername);
        group_info->insert(std::pair<std::string,std::list<std::string>>(groupname,l));
        l.clear();
    }
}
void ChatInfo::list_print_group()
{
   for (auto it = group_info->begin();it!=group_info->end();it++)
   {
        std::cout<<it->first<<" ";
        for(auto i =it ->second.begin();i!=it->second.end();i++)
        {
            std::cout<<*i <<" ";
        }
        std::cout<<std::endl;
   }
}
bool ChatInfo:: list_updata_list(Json::Value &v,struct bufferevent *bev)
{   
    struct User u ={v["username"].asString(),bev};
    std::unique_lock <std::mutex>lck(List_mutex);

    online_user->push_back(u);
    return true;
}
struct bufferevent * ChatInfo:: list_friend_online(std::string n)
{
    std::unique_lock <std::mutex>lck(List_mutex);
    for(auto it =online_user->begin();it!=online_user->end();it++)
    {
        if(it->name == n)
        {
            return it->bev;
            break;
        }
    }
    return NULL;
}
 bool ChatInfo:: list_group_is_exist(std::string name)
 {

    std::unique_lock<std::mutex>map_mutex;
    for(auto it = group_info->begin();it!=group_info->end();it++)
    {
        if(it->first==name)
        {
            return true;
        }
    }
    return false;

 }
 void ChatInfo:: list_add_new_group(std::string g,std::string owner)
 {
     
     std::list<std::string>l;
     l.push_back(owner);
     std::unique_lock<std::mutex>map_mutex;
     group_info->insert(make_pair(g,l));
 }
  bool ChatInfo:: list_member_is_group(std::string g,std::string u)
  {
    for(auto it =group_info->begin();it!=group_info->end();it++)
    {
        if(it->first == g)
        {
            for(auto i =it->second.begin();i != it->second.end();i++)
            {
                if(*i == u)
                {
                    return true;
                }
            }
            return false;
        }
    }
    return false;
  }
void ChatInfo:: list_update_group_member(std::string g,std::string u)
{
      std::unique_lock<std::mutex>map_mutex;
        for(auto it =group_info->begin();it!=group_info->end();it++)
    {
        if(it->first == g)
        {
            it->second.push_back(u);
        }
    }
}
std::list<std::string> &ChatInfo:: list_get_list(std::string g)
{
    auto it =group_info->begin();
     for(;it!=group_info->end();it++)
    {
        if(it->first == g)
        {
           break;
        }
    }
    return it->second;
}
void ChatInfo:: list_delete_user(std::string username )
{
    std::unique_lock<std::mutex>list_mutex;
    auto it =online_user->begin();
     for(;it!=online_user->end();it++)
    {
        if(it->name == username)
        {
           online_user->erase(it);
           return;
        }
    }
}