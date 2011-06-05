/*********************************************
 * File name: proc.h
 * Author: Cassidy
 * Time-stamp: <2011-05-26 18:23:53>
 *********************************************
 */

#ifndef _PROC_H
#define _PROC_H

#include <kernel/kernel.h>
#include <kernel/head.h>        /* desc_struct */

#define RUNNING         0
#define SLEEPING        1
#define ZOMBIE          2

#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY 5

/* 第 n 个 TSS 和 LDT 在全局描述符表的偏移位置 */
#define TSS(n) ((((unsigned long)(n))<<4) + (FIRST_TSS_ENTRY<<3))
#define LDT(n) ((((unsigned long)(n))<<4) + (FIRST_LDT_ENTRY<<3))

#define ltr(n) __asm__("ltr %%ax"::"a"(TSS(n))) /* 加载第 n 个任务状态段寄存器 */
#define lldt(n) __asm__("lldt %%ax"::"a"(LDT(n))) /* 加载第 n 个局部描述符表寄存器 */

struct i387_struct {
  long cwd;
  long swd;
  long twd;
  long fip;
  long fcs;
  long foo;
  long fos;
  long st_space[20];
};

/* 任务状态段结构体 */
struct tss_struct {
  long back_link;         /* 先前任务链接字段，含有前一个任务 TSS 段选择符 */
  long esp0;
  long ss0;
  long esp1;
  long ss1;
  long esp2;
  long ss2;
  long cr3;
  long eip;
  long eflags;
  long eax, ecx, edx, ebx;
  long esp;
  long ebp;
  long esi;
  long edi;
  long es;
  long cs;
  long ss;
  long ds;
  long fs;
  long gs;
  long ldt;
  long trace_bitmap;      /* 31...16 -- I/O位图基地址， 0 -- 调试陷阱标志 */
  struct i387_struct i387;
};

/* 进程结构体 */
struct proc_struct {
  int proc_type;          /* 进程类型(0-无效, 1-Task, 2-Server, 3-User) */
  long state;             /* 运行状态 */
  long counter;           /* 运行时间计数 */
  int exit_code;          /* 进程停止执行后的退出码 */
  unsigned long start_code;
  unsigned long end_code;
  unsigned long end_data;
  unsigned long brk;      /* 总长度（字节数） */
  unsigned long start_stack;
  long pid;               /* 进程号 */
  long father;            /* 父进程号 */
  long pgrp;              /* 进程组号 */
  long session;           /* 会话号 */
  long leader;            /* 会话首领号 */
  unsigned uid;           /* 用户 id */
  unsigned euid;          /* 有效用户 id */
  unsigned suid;          /* 保存的用户 id */
  unsigned gid;           /* 组 id */
  unsigned egid;          /* 有效组 id */
  unsigned sgid;          /* 保存的组 id */
  long alarm;             /* 用户态运行时间(滴答数) */
  long utime;             /* 内核态运行时间(滴答数) */
  long stime;
  long cutime;
  long cstime;
  long start_time;        /* 进程开始运行时刻 */
  unsigned short used_math;         /* 是否使用了协处理器 */
  int tty;
  struct proc_struct * next_ready;  /* 在进程队列的下一个进程 */
  /* 局部描述符表(0-空, 1-代码段, 2-数据段) desc_struct -- include/kernel/head.h */
  struct desc_struct ldt[3];
  struct tss_struct tss;            /* 进程的任务状态段信息结构 */
};

#define switch_to(n) {                          \
  struct {long a,b;} _tmp;                      \
  __asm__("cmpl %%ecx, proc_current\n\t"        \
      "je 1f\n\t"                               \
      "movw %%dx, %1\n\t"                       \
      "xchgl %%ecx, proc_current\n\t"           \
      "ljmp %0\n\t"                             \
      "1:"                                      \
      ::"m"(*&_tmp.a), "m"(*&_tmp.b),           \
      "d"(TSS(n)), "c"((long)proc[n]));         \
  }

#define INIT_PROC_DATA                          \
  {0, 0, 0,                                     \
      0, 0, 0, 0, 0, 0,                         \
      0, -1, 0, 0, 0,                           \
      0, 0, 0, 0, 0, 0,                         \
      0, 0, 0, 0, 0, 0,                         \
      0,                                        \
      -1,                                       \
      NULL,                                     \
        {                                       \
          {0, 0},                               \
            {0, 0},                             \
              {0, 0}                            \
        },                                      \
          {0, 0, 0x10,                          \
              0, 0, 0, 0,                       \
              PAGE_DIR_ADDR,                    \
              0, 0, 0, 0, 0,                    \
              0, 0, 0, 0, 0,                    \
              0x17,0x0F,0x17,0x17,0x17,0x17,    \
              0, 0x80000000,                    \
              {}                                \
          }    }

#endif
