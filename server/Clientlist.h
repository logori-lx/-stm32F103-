/**
 * @file Clientlist.h
 * @brief 用于存储有关用户链表的结构声明以及函数声明
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
#ifndef __CLIENTLIST_H__
#define __CLIENTLIST_H__
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

/**
 * @brief 用户链表结构体节点声明
 */
typedef struct clientNode{
    int connfd;
    struct clientNode *next;
}clientListNodeStruct;

/**
 * @brief 用户链表头节点全局变量，整个项目中默认只有一个用户链表
 */
extern clientListNodeStruct *client_list;


void addToClientList(int connfd, clientListNodeStruct **head);
bool deleteNodeClientList(int connfd, clientListNodeStruct **head);

# endif