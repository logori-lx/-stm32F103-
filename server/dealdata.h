/**
 * @file dealdata.h
 * @brief 用于存储与数据收发相关的宏、全局变量以及函数声明
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

#ifndef __DEALDATA_H__
#define __DEALDATA_H__
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
#define MAX_CMD_STR 100
///@brief 定义在main函数中，标识服务器的运行状态，当该状态位为真，所有线程都应该停止并退出
extern bool stop_srv;
///@brief 定义在main函数中，标识如果产生中断时，引起中断的类型
extern int sig_type;

char* readData(int connfd, char *buf);
bool writeData(int connfd, char *message);
#endif
