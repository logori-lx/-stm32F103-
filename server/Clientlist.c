/**
 * @file Clientlist.c
 * @brief 对用户链表进行处理，服务器端在转发设备端的数据时，会遍历用户链表，向链表上的所有用户
 *        发送设备所上传的信息。实现一对多通信。此文件存储将用户挂载到用户链表上或者从用户链表
 *        上删除用户的处理函数
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
#include"Clientlist.h"

/**
 * @brief 实现将用户挂载到用户链表上的函数
 * @param  connfd   要挂载的用户的套接字
 * @param  head     被挂载的用户链表头
 */
void addToClientList(int connfd, clientListNodeStruct **head)
{
    clientListNodeStruct *Node = (clientListNodeStruct *)malloc(sizeof(clientListNodeStruct));
    clientListNodeStruct *temp;
    Node->connfd = connfd; 
    Node->next = NULL;
    if(*head == NULL)
        *head = Node;
    else{
        clientListNodeStruct *point = *head;
        while(point->next != NULL)  point = point->next;
        point->next = Node;
    }
    printf("The list is: ");
    for(temp = *head; temp != NULL; temp = temp->next)
    {
        printf("%d ",temp->connfd);
    }
    printf("\n");
}
/**
 * @brief 用于从用户链表上删除特定套接字的用户
 * @param  connfd  要删除的特定套接字的用户
 * @param  head    要进行处理的用户链表头结点
 * @return true     成功进行用户链表上的用户删除
 * @return false    用户链表为空，删除失败
 */
bool deleteNodeClientList(int connfd, clientListNodeStruct **head)
{
    if(connfd == (*head)->connfd)
    {
        free(*head);
        (*head) = NULL;
        return true;
    }
    clientListNodeStruct *point = *head, *nextPoint = point->next;
    while(nextPoint != NULL)
    {
        if(nextPoint->connfd == connfd)
        {
            clientListNodeStruct *temp = nextPoint;
            point->next = temp->next;
            free(nextPoint);
            return true;
        }        
        nextPoint = nextPoint->next;
        point = point->next;
    }
    return false;
}