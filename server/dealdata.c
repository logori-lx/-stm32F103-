/**
 * @file dealdata.c
 * @brief 用于存储数据收发相关的处理函数，以使服务器支持自定义的边界访问协议（先发送要发送的信息的长度，再发送要发送的信息）
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
#include"dealdata.h"
/**
 * @brief 以自定义的边界协议（先发送要发送的信息的长度，再发送要发送的信息）向特定的套接字发送信息
 * @param  connfd  要接收发送信息的套接字       
 * @param  message  要发送的消息
 * @return true  表明消息发送成功
 * @return false 表明消息发送失败
 */
bool writeData(int connfd, char *message)
{
    char *buffer;
    int res,sum;
    int len_n = 0, len_h = 0;
    int read_num  = 0;
    //缓冲区一定要清空，否则会有其他垃圾被读进去
    len_h = strlen(message) + 1;
    if(len_h > MAX_CMD_STR + 1)
    {
        printf("[srv] read length error! len_h(%d) is bigger than MAX_CMD_STR+1!\n",len_h);
        return false;
    }
    len_n = htonl(len_h);
    res = write(connfd,&len_n,sizeof(len_n));
    res = write(connfd,message,len_h);
}
/**
 * @brief 用于从特定的套接字接收信息
 * @param  connfd  要接收信息的套接字
 * @param  buf     要存储接收信息的缓冲区
 * @return char* 存储了接收信息的缓冲区地址
 */
char* readData(int connfd, char *buf)
{
    char *buffer;
    int res,sum;
    int len_n = 0, len_h = 0;
    int read_num  = 0;
    //缓冲区一定要清空，否则会有其他垃圾被读进去
    while(! stop_srv)
    {
        res = read(connfd,(void *)&len_n, sizeof(len_n));
        if(res < 0){
            printf("[srv] read len return %d and errno is %d\n", res, errno);
            perror("");
            if(errno == EINTR){
                if(sig_type == SIGINT)
                    return NULL;//但凡收到SIGINT，指示服务器结束
                else
                    continue;//若是其他信号中断，则重新回到read读出数据长度的阶段。
            }
            else
                return NULL;
            }
        else if(res == 0)
            return NULL;
        len_h = ntohl(len_n);
        if(len_h > MAX_CMD_STR + 1)
        {
            printf("[srv] read length error! len_h(%d) is bigger than MAX_CMD_STR+1!\n",len_h);
            return NULL;
        }
        sum = 0;
        do{
            res = read(connfd, (void *)&buf[sum], len_h - sum);
            if(res < 0){
                printf("[srv] read len return %d and errno is %d\n", res, errno);
                if(errno == EINTR){
                    if(sig_type == SIGINT)
                        return NULL;//但凡收到SIGINT，指示服务器结束
                    else
                        continue;//若是其他信号中断，则重新执行读取
                }
                else
                return NULL;
            }
            else if(res == 0 && len_h != 0)
            return;
            else 
            sum += res;
        }while(sum < len_h);
        return buf;
    }
}