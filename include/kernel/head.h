/*********************************************
 * File name: head.h
 * Author: Cassidy
 * Time-stamp: <2011-04-24 06:20:36>
 *********************************************
 */

#ifndef _HEAD_H
#define _HEAD_H

typedef struct desc_struct
{
  unsigned long a, b;
} desc_table[256];

extern unsigned long pg_dir[1024];
extern desc_table idt, gdt;

#endif
