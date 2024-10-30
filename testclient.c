#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <json/json.h>
void *xx(void *arg)
{
    int sockfd=*(int *)arg;
    char buf[1024]={0};
    int len;
    while(1)
    {
        recv(sockfd,&len,4,0);
        printf("收到长度 %d",len);
        recv(sockfd,buf,len,0);
        printf("收到数据 %s\n",buf);
        memset(buf,0,1024);
        sleep(10);
    }
}
int main()
{   
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //  printf("444444444444444\n");
    if (sockfd < 0) 
    {
        perror("socket creation failed");
        exit(1);
    }

    struct sockaddr_in server_info;
    memset(&server_info, 0, sizeof(server_info));
    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = inet_addr("10.0.20.16");
    server_info.sin_port = htons(8888);
  //printf("33333333333333\n");
    if (connect(sockfd, (struct sockaddr*)&server_info, sizeof(server_info)) == -1)
    {
        perror("connect");
        close(sockfd); // 确保关闭套接字
        exit(1);
    }
    pthread_t tid;
    pthread_create(&tid,NULL,xx,&sockfd);
    Json :: Value val;
    val["cmd"]="private";
    val["username"]="tom";
    val["friend"]="小王";
    std::string s =Json::FastWriter().write(val);
    char buf[124]={0};
    int len =s.size();
    memcpy(buf,&len,4);
    memcpy(buf+4,s.c_str(),len);
    send(sockfd,buf,len+4,0);
    while(1);
    close(sockfd); // 关闭套接字
    return 0;
}