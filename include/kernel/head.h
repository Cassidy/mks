/*********************************************
 * File name: head.h
 * Author: Cassidy
 * Time-stamp: <2011-04-24 06:20:36>
 *********************************************
 */

#ifndef _HEAD_H
#define _HEAD_H

typedef struct desc_struct {
  unsigned long a,b;
} desc_table[256];

/* 描述符结构体 */
struct desc_struct {
  unsigned long a, b;
};

/* 定义一个新的类型 desc_table
   指明 desc_table 是一个有 256 个元素的 desc_struct 类型数组名 */
typedef struct desc_struct desc_table[256];

extern unsigned long pg_dir[1024];
extern desc_table idt, gdt;

#endif
