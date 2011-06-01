/*********************************************
 * File name: head.h
 * Author: Cassidy
 * Time-stamp: <2011-04-24 06:20:36>
 *********************************************
 */

#ifndef _HEAD_H
#define _HEAD_H

/* 描述符结构体 */
struct desc_struct {
  unsigned long a, b;
};

extern struct desc_struct *idt; /* kernel/interrupt.c */
extern struct desc_struct *gdt; /* kernel/proc.c */

extern unsigned long *pg_dir;   /* kernel/memory.c */


#endif
