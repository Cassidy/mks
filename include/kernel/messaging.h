/*********************************************
 * File name: messaging.h
 * Author: Cassidy
 * Time-stamp: <2011-05-15 01:23:56>
 *********************************************
 */

#ifndef _MESSAGING_H
#define _MESSAGING_H

#define MSG_ANY -1
#define MSG_HARDWARE -2

#define NR_MSG_HARDWARE 1024    /* 硬件中断消息数量 */

/* 消息结构体 */
struct msg_struct
{
  long src;                     /* 消息的源进程 */
  long dest;                    /* 消息的目的进程 */
  long msg;                     /* 短消息内容或是长消息指针 */
  struct msg_struct * next;
};


#endif
