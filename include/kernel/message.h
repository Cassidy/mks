/*********************************************
 * File name: message.h
 * Author: Cassidy
 * Time-stamp: <2011-05-19 20:01:13>
 *********************************************
 */

#ifndef _MESSAGE_H
#define _MESSAGE_H

//消息是两页大小，其中只有与页面对齐的一个完整页面的数据是有效的
struct Msg
{
  char a[4096*2];
};


#endif