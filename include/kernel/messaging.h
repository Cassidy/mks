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

#define NR_MSG_HARDWARE 500       //硬件中断消息数量

struct msg_struct
{
  long src;
  long dest;
  long msg;
  struct msg_struct * next;
};


#endif
