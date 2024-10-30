#include<iostream>
using namespace std;
#include"chat_server.h"
int main()
{
    //创建服务器对象
    ChatServer s;
    s.listen(IP,PORT);

    return 0;
}