/**
 * @file server.c
 * @brief 存储服务器运行主体函数的文件
 * @author 楼旭 (1731477306.com)
 * @version 1.0
 * @date 2020-11-16
 * 
 * @copyright Copyright (c) 2020 楼旭带专职业有限公司
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2020-11-16 <td>1.0     <td>wangh     <td>内容
 * </table>
 */
#include<sys/types.h>      //像AF_INET这些参数，如果函数用到了的话都需要加这个头文件
#include<sys/socket.h>     //用到的函数socket，如果函数需要传递sockfd参数，则也需要用到这个头文件
#include<stdlib.h>         //用到的函数calloc,函数atoi
#include<stdio.h>
#include<signal.h>         //sigaction函数及sigaction结构体以及其他宏定义
#include<string.h>         //strlen、strcpy、bzero
#include<unistd.h>         //write、read、close
#include<netinet/in.h>     //存放 struct sockaddr_in
#include<arpa/inet.h>      //inet_pton函数
#include<stdbool.h>        //用于定义bool类型变量
#include<errno.h>          //用于变量errno
#include<pthread.h>
#include"dealdata.h"
#include"Clientlist.h"
///@brief 标识服务器的运行状态
bool stop_srv = false;
///@brief 标识中断信号的种类
int sig_type;
///@brief 标识用户线程变量
pthread_t clientThread;
///@brief 标识设备线程变量
pthread_t deviceThread;
///@brief 创建用户链头结点
clientListNodeStruct *client_list = NULL;
///@brief 定义设备套接字，默认项目中的设备只有一个
int deviceConnfd = 0;

/**
 * @brief 用于处理SIGINT信号的信号处理函数
 * @param  signo  信号变量，用于设置sig_type，判断信号的类型
 */
void sig_int(int signo)
{
    printf("[srv] SIGINT is comming!\n");
    stop_srv = true;
    sig_type = signo;
    return;
}
/**
 * @brief 用于处理SIGPIPE信号的信号处理函数
 * @param  signo  信号变量，用于设置sig_type，判断信号的类型          
 */
void sig_pipe(int signo)
{
    sig_type = signo;
    int num;
    printf("[srv] SIGPIPE is coming!\n");
}

/**
 * @brief 用于处理设备的连接，对设备数据进行处理后进行接收及转发
 * @param  connfd 设备的套接字          
 */
void deal_device(int connfd)
{
    char *buffer;
    int res,sum;
    int len_n = 0, len_h = 0;
    int read_num  = 0;
    //缓冲区一定要清空，否则会有其他垃圾被读进去
    char buf[MAX_CMD_STR + 1] = {0};
    printf("Hello I'm device\n");
    deviceConnfd = connfd;
    while(! stop_srv)
    {   
        char *message = readData(connfd,buf);
        if(message == NULL)
        {
            printf("[srv] Device %d disconnected!\n",connfd);
            close(connfd);
            return;
        }
        else if(strstr(message, "I"))
        {
            /*
            int i;
            for(i = 0;;i++)
            {
                if(message[i] == '#')
                {
                    message[i] = '\0';
                    break;
                }
            }*/
            //将#换成\0
            message[strlen(message) - 1] = '\0';
            clientListNodeStruct *temp;
            for(temp = client_list; temp != NULL; temp = temp->next)
            {
                writeData(temp->connfd,message);
            }
            printf("[echo_rqt] %s\n",message);
        }
        else if(strstr(message, "K"))
        {
            //将#换成\0
            message[sizeof(message) - 1] = '\0';
            clientListNodeStruct *temp;
            for(temp = client_list; temp != NULL; temp = temp->next)
            {
                writeData(temp->connfd,message);
            }
            printf("[echo_rqt] %s\n",message);
        }
        else{
            clientListNodeStruct *temp;
            for(temp = client_list; temp != NULL; temp = temp->next)
            {
                writeData(temp->connfd,message);
            }
            printf("[echo_rqt] %s\n",message);
        }
    }

}
/**
 * @brief 用于处理客户端的连接，将数据进行处理后进行数据的准发
 * @param  connfd   客户端的套接字   
 */
void deal_client(int connfd)
{
    char *buffer;
    int res,sum;
    int len_n = 0, len_h = 0;
    int read_num  = 0;
    //缓冲区一定要清空，否则会有其他垃圾被读进去
    char buf[MAX_CMD_STR + 1] = {0};
    printf("Hello I'm client!\n");
    addToClientList(connfd, &client_list);
    printf("list connfd is %d\n",client_list->connfd);
    printf("connfd is %d\n",connfd);
    while(!stop_srv)
    {
        char *message = readData(connfd,buf);
        if(message == NULL)
        {
            if(deleteNodeClientList(connfd,&client_list))
            {
                printf("[srv] Client %d disconnected!\n",connfd);
                close(connfd);
            }
            return;
        }
        printf("[echo_rqt] %s\n",message);
        write(deviceConnfd,message,strlen(message) + 1);
        //writeData(deviceConnfd,message);
    }
}
//这个函数是从echo_rep函数中复制过来的，如果可以的话，可以封装一下，成为一个新函数
//用于判断连接的设备是设备还是客户端。
/**
 * @brief 对连接的客户端进行处理，判断是设备端还是客户端，并分别调用相应的函数创建客户线程或者设备进程进行处理
 * @param  connfd 接入的客户端套接字          
 */
void deal_message(int connfd)
{
    char buf[MAX_CMD_STR + 1] = {0};
    char *message;
    clientListNodeStruct *temp;
    message = readData(connfd,buf);
    //当为空表明对方断开连接
    if(message == NULL)
    {
        printf("[srv] Unknown client disconnected!\n");
        return;
    }
    printf("[echo_rqt] %s\n",message);
    if(!strcmp(message,"client"))
    {
        pthread_create(&clientThread,NULL,(void *)&deal_client,(void *) connfd);
    }
    else if(!strcmp(buf,"device"))
    {
        for(temp = client_list; temp != NULL; temp = temp->next)
            writeData(temp->connfd,"Device is connected!\n");
        pthread_create(&deviceThread,NULL,(void *)&deal_device,(void *) connfd);
    }
    
}

/**
 * @brief 服务器运行所要执行的主体函数
 * @param  argc             
 * @param  argv             
 * @return int 
 */
int main(int argc, char* argv[])
{
    int port = atoi(argv[2]);
    struct sigaction sigact_int, old_sigact_int, sigact_pipe, old_sigact_pipe;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    int listenfd, connfd;
    char buf[MAX_CMD_STR + 1];

    if(argc != 3)
    {
        printf("Usage:%s <IP> <PORT>\n",argv[0]);
        exit(-1);
    }

    sigact_pipe.sa_handler = sig_pipe;
    sigemptyset(&sigact_int.sa_mask);
    sigact_pipe.sa_flags = 0;
    sigact_pipe.sa_flags |= SA_RESTART;
    sigaction(SIGPIPE,&sigact_pipe,&old_sigact_pipe);

    sigact_int.sa_handler = sig_int;
    sigemptyset(&sigact_int.sa_mask);
    sigact_int.sa_flags = 0;
    sigaction(SIGINT,&sigact_int, &old_sigact_int);


    if((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("[srv] Error with socket()!\n");
        return -1;
    }
    
    //设置服务器地址及端口号信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr) == 0)
    {
        printf("[srv] Error with inet_pton()!\n");
        return -1;
    }
    bzero(&server_addr.sin_zero, 8);
    printf("[srv] server[%s:%d] is initializing!\n",argv[1], port);
    if(bind(listenfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1)
    {
        printf("[srv] Error with bind()!\n");
        perror("");
        printf("%d",errno);
        return -1;
    }

    if(listen(listenfd,1) == -1)
    {
        printf("[srv] Error with listen()!\n");
        return -1;
    }
    int addr_length = sizeof(struct sockaddr);
    //client_addr.sin_port = htons(port);
    while( !stop_srv)
    {
        printf("here");
        connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addr_length);
        if(connfd == -1 && errno == EINTR)
        {
            printf("[srv] accept return -1 and errno is EINTR\n");
            close(listenfd);
            printf("[srv] listenfd is closed\n");
            break;
        }
        else
        {
            printf("[srv] client[%s:%d] is accepted!\n",inet_ntop(AF_INET, &client_addr.sin_addr,buf,MAX_CMD_STR), ntohs(client_addr.sin_port));
            deal_message(connfd);
        }
    }
    close(connfd);
    printf("[srv] connfd is closed!\n"); 
    printf("[srv] server is going to exit!\n");
}