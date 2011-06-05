/*********************************************
 * File name: config.h
 * Author: Cassidy
 * Time-stamp: <2011-05-17 03:12:23>
 *********************************************
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#define HZ 100
/* PC8253 定时芯片的输入时钟频率约为 1.193180MHZ。
   这里设置 8253 芯片发出中断的频率是 100HZ，也就是
   10ms 发出一次时钟中断 */
#define LATCH (1193180/HZ) 
#define GDT_ADDR 0x6800
#define IDT_ADDR 0x6000
#define PAGE_DIR_ADDR 0x7000
#define COUNTER 10

#define PROC_IDLE_NR 0
#define PROC_INIT_NR 1
#define PROC_SYSTEM_NR 2

#define NR_PROCS 64
#define NR_INIT_TASKS 1
#define NR_INIT_SERVERS 0
#define NR_INIT_USERS 2
#define NR_INIT_PROCS (NR_INIT_TASKS + NR_INIT_SERVERS + NR_INIT_USERS)    //初始化进程数,包括任务数和服务器数

/* idle 进程的类型是 3（用户进程），标志寄存器设置了 IF 中断标志
   init_proc 进程的类型是 3（用户进程），标志寄存器设置了 IF 中断标志
   system_task 进程的类型是 1（任务进程），标志寄存器设置了 IF 中断标志和 IOPL */

#define PROC_INIT                                       \
  {                                                     \
  proc[PROC_IDLE_NR]->proc_type = 3;                    \
  proc[PROC_IDLE_NR]->tss.eip = (long)&idle;            \
  proc[PROC_IDLE_NR]->tss.eflags = 0x00000202;          \
  proc[PROC_INIT_NR]->proc_type = 3;                    \
  proc[PROC_INIT_NR]->tss.eip = (long)&init_proc;       \
  proc[PROC_INIT_NR]->tss.eflags = 0x00000202;          \
  proc[PROC_SYSTEM_NR]->proc_type = 1;                  \
  proc[PROC_SYSTEM_NR]->tss.eip = (long)&system_task;   \
  proc[PROC_SYSTEM_NR]->tss.eflags = 0x00003202;        \
  }

#endif
