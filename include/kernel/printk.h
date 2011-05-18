/*********************************************
 * File name: printk.h
 * Author: Cassidy
 * Time-stamp: <2011-05-10 21:40:02>
 *********************************************
 */

#ifndef _PRINTK_H
#define _PRINTK_H

extern void con_init(void);                     //显示初始化
extern int printk(const char *);                //输出字符串
extern int prints(const char *);                //输出字符串
extern void printa(const long a);               //输出长整型变量
extern void printbin(const unsigned long);      //输出二进制数

#endif
