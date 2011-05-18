/*********************************************
 * File name: unistd.h
 * Author: Cassidy
 * Time-stamp: <2011-05-14 20:59:18>
 *********************************************
 */

#ifndef _UNISTD_H
#define _UNISTD_H

#include <kernel/message.h>

#ifndef NULL
#define NULL ((void *) 0)     //定义空指针
#endif

extern int errno;

long small_msg_send(long destination, long * msgpt);
long small_msg_receive(long source, long * msgpt);
long big_msg_send(long destination, struct Msg * msgpt);
long big_msg_receive(long source, struct Msg * msgpt);

int fork(void);

#endif
