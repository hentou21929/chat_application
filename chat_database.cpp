#include "chat_database.h"
DataBase ::DataBase()
{

}
DataBase :: ~DataBase()
{

}
bool DataBase :: database_connent()
{
    //初始化数据库句柄
    mysql = mysql_init(NULL);
    //连接数据库
    mysql = mysql_real_connect(mysql,"localhost","root","","chat_database",0,NULL,0);
    if(NULL==mysql)
    {
        std::cout<<"mysql error"<<std::endl;
        return false;
    }
    //设置编码格式
    if( mysql_query(mysql,"SET NAMES 'utf8'")!=0)
    {
        std::cout<<"std name utf8 error"<<std::endl;
        return false;
    }
    return true;
}
bool DataBase:: database_init_table()
{
    database_connent();
    const char *g=  "CREATE TABLE if not exists chat_group(groupname varchar(128),groupowner varchar(128),groupmember varchar(128))charset utf8;";

    if(mysql_query(mysql,g)!=0)
    {
        return false;
    }
    const char * u="CREATE TABLE if not exists chat_user(username varchar(128),password varchar(128),friendlist varchar(4096),grouplist varchar(4096))charset utf8;";
    if(mysql_query(mysql,u)!=0)
    {
        return false;
    }
    database_disconnect();
    return true;
}
void DataBase::database_disconnect()
{
    mysql_close(mysql);

}
int DataBase:: database_get_group_info(std::string *g)
{
    if(mysql_query(mysql,"select *from chat_group;")!=0)
    {
        std::cout<<"select error"<<std::endl;
        return -1;
    }
   MYSQL_RES *res= mysql_store_result(mysql);
   if(NULL==res)
   {
        std::cout<<"store result error "<<std::endl;
        return-1;
   }
    MYSQL_ROW r;
    int idx=0;
    while(r=mysql_fetch_row(res))
    {
        g[idx]+=r[0];
        g[idx]+="|";
        g[idx]+=r[2];
        //std::cout<<g[idx]<<std::endl;
        idx++;

    }
    mysql_free_result(res);
    return idx;
}
bool DataBase::database_user_is_exist(std::string  u)
{
    char sql[256]={0};
    sprintf(sql,"select * from chat_user where username ='%s';",u.c_str());
    std::unique_lock<std::mutex>lck(_mutex);
    if(mysql_query(mysql,sql)!=0)
    {
        std::cout<<"select error"<<std::endl;
        return true;
    }
    MYSQL_RES*res= mysql_store_result(mysql);
    if(res==NULL)
    {
        std::cout<<"store result error"<<std::endl;
        return true;
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    if(NULL==row)
    {
       // std::cout<<"用户不存在"<<std::endl;
        return false;

    }else 
    {
       // std::cout<<"用户存在"<<std::endl;
        return true;
    }

}
void DataBase:: database_insert_user_info(Json::Value &v)
{
    std::string username=v["username"].asString();
    std::string password=v["password"].asString();
    char sql[256]={0};
    std::unique_lock<std::mutex>lck(_mutex);
    sprintf(sql,"insert into chat_user (username,password) values('%s','%s');",username.c_str(),password.c_str());
    if(mysql_query(mysql,sql)!=0)
    {
        std::cout<<"inser into error"<<std::endl;
        return ;
    }
}
bool DataBase:: database_password_correct(Json::Value &v)
{
    std::string username = v ["username"].asString();
    std::string password = v ["password"].asString();
    std::unique_lock<std::mutex>lck(_mutex);
    char sql[256]={0};
    std::cout<<"密码："<<password<<std::endl;
    sprintf(sql,"select password from chat_user where username= '%s';",username.c_str());
    if(mysql_query(mysql,sql)!=0)
    {
        std::cout<<"select password error" <<std::endl;
        return false;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    if(res==NULL)
    {
        std::cout<< "mysql store result error"<<std::endl;
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    if(NULL==row)
    {
        std::cout<<"fetch row error "<<std::endl;
        return false;
    }
    if(!strcmp(row[0],password.c_str()))
    {
        return true;
    }else 
    {
        return false;
    }
}
bool DataBase:: database_get_friend_group(Json::Value &v,std::string &friList,std::string &groList)
{
     // 验证输入的用户名
    if (!v.isMember("username") || v["username"].asString().empty()) {
        std::cout << "Invalid username" << std::endl;
        return false;
    }

    std::string username = v["username"].asString();
    char sql[1024] = {0};
    sprintf(sql, "SELECT * FROM chat_user WHERE username = '%s';", username.c_str());

    std::unique_lock<std::mutex> lck(_mutex);
    if (mysql_query(mysql, sql) != 0) {
        std::cout << "MySQL query error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    if (res == NULL) {
        std::cout << "mysql_store_result error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL) {
        std::cout << "No results found for username: " << username << std::endl;
        mysql_free_result(res);
        return false;
    }

    // 检查 row 的字段是否有效
    if (row[2] != NULL) {
        friList = row[2];
    } else {
        friList.clear(); // 或处理为默认值
    }

    if (row[3] != NULL) {
        groList = row[3];
    } else {
        groList.clear(); // 或处理为默认值
    }

    mysql_free_result(res); // 释放结果
    return true;

}
void DataBase:: database_add_friend(Json::Value &v)
{
    std::string username=v["username"].asString();
    std::string friendname=v["friendname"].asString();
    database_up_friendlist(username,friendname);
    database_up_friendlist(friendname,username);

}
void DataBase:: database_up_friendlist(std::string &u,std::string &f)
{
    char sql[256]={0};
    sprintf(sql,"select friendlist from chat_user where username = '%s';",u.c_str());
    std::string friendlist;
    std::unique_lock<std::mutex>lck(_mutex);
    if(mysql_query(mysql,sql)!=0)
    {
        std::cout<<"select friendlist error"<<std::endl;
        return;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    if(NULL==res)
    {
        std::cout<<"store result error "<<std::endl;
        return;
    }
    MYSQL_ROW row=mysql_fetch_row(res);
    if(row[0]==NULL)
    {
        friendlist.append(f);
    }else
    {
        friendlist.append(row[0]);
        friendlist.append("|");
        friendlist.append(f);
    }
    memset(sql,0,sizeof(sql));
    sprintf(sql,"update chat_user set friendlist ='%s' where username='%s';",f.c_str(),u.c_str());
    if(mysql_query(mysql,sql)!=0)
    {
        std::cout<<"update error"<<std::endl;

    }
}
void DataBase:: add_new_group(std::string g,std::string owner)
{
    char sql[256]={0};
    sprintf(sql,"insert into chat_group values ('%s','%s','%s');",g.c_str(),owner.c_str(),owner.c_str());
    std::unique_lock<std::mutex>lck(_mutex);
    if(mysql_query(mysql,sql)!=0)
    {
        std::cout<<"inser error"<<std::endl;
        return;
    }

}
void DataBase:: database_updata_group_member(std::string g,std::string u)
{
    //先把数据读出来

    char sql[256]={0};
    std::string member;
    sprintf(sql,"select groupmember from chat_group where groupname = '%s';",g.c_str());
    std::unique_lock<std::mutex>lck(_mutex);
    if(mysql_query(mysql,sql)!=0)
    {
        std::cout<<"select error"<<std::endl;
        return;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    if(NULL==res)
    {
        std::cout<<"store result error "<<std::endl;
        return;
    }
    MYSQL_ROW row=mysql_fetch_row(res);
    if(row ==NULL)
    {

    }else 
    {
        member.append(row[0]);
        member.append("|");
        member.append(u);
    }
    mysql_free_result(res);
    //修改数据库
    memset(sql,0,sizeof(sql));
    sprintf(sql,"update chat_group set groupmember = '%s' where groupname='%s';",member.c_str(),g.c_str());
    if(mysql_query(mysql,sql)!=0)
    {
        std::cout<<"update error"<<std::endl;
        return;
    }
}