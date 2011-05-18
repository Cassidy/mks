/*********************************************
 * File name: proc.h
 * Author: Cassidy
 * Time-stamp: <2011-05-14 22:52:32>
 *********************************************
 */

#ifndef _PROC_H
#define _PROC_H

#include <kernel/kernel.h>
#include <kernel/head.h>

#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY 5
#define TSS(n) ((((unsigned long)n)<<4) + (FIRST_TSS_ENTRY<<3))
#define LDT(n) ((((unsigned long)n)<<4) + (FIRST_LDT_ENTRY<<3))

#define RUNNING 0
#define SLEEPING 1

struct i387_struct
{
  long cwd;
  long swd;
  long twd;
  long fip;
  long fcs;
  long foo;
  long fos;
  long st_space[20];
};

struct tss_struct
{
  long back_link;
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
  long trace_bitmap;
  struct i387_struct i387;
};

struct proc_struct
{
  int proc_type;          //0:无效,1:Task,2:Server,3:User
  long state;
  long counter;
  int exit_code;
  unsigned long start_code;
  unsigned long end_code;
  unsigned long end_data;
  unsigned long brk;     //总长度（字节数）
  unsigned long start_stack;
  long pid;
  long father;
  long pgrp;
  long session;
  long leader;
  unsigned short uid;
  unsigned short euid;
  unsigned short suid;
  unsigned short gid;
  unsigned short egid;
  unsigned short sgid;
  long alarm;
  long utime;
  long stime;
  long cutime;
  long cstime;
  long start_time;
  unsigned short used_math;
  int tty;
  struct proc_struct * next_ready;
  struct desc_struct ldt[3];
  struct tss_struct tss;
};

#define INIT_PROC_DATA	\
  {0, 0, 0,		\
      0, 0, 0, 0, 0, 0,	\
      0, -1, 0, 0, 0,	\
      0, 0, 0, 0, 0, 0,	\
      0, 0, 0, 0, 0, 0,	\
      0,		\
      -1,		\
      NULL,		\
	{		\
	  {0, 0},	\
	    {0, 0},     \
	      {0, 0}	\
	},		\
	  {0, 0, 0x10,	\
	      0, 0, 0, 0,			\
	      (long)&pg_dir,			\
	      0, 0, 0, 0, 0,			\
	      0, 0, 0, 0, 0,			\
	      0x17,0x0F,0x17,0x17,0x17,0x17,	\
	      0, 0x80000000,			\
	      {}				\
	  }    }

#define ltr(n) __asm__("ltr %%ax"::"a"(TSS(n)))
#define lldt(n) __asm__("lldt %%ax"::"a"(LDT(n)))

#define switch_to(n) {				\
  struct {long a,b;} _tmp;			\
  __asm__("cmpl %%ecx, proc_current\n\t"	\
	  "je 1f\n\t"				\
	  "movw %%dx, %1\n\t"			\
	  "xchgl %%ecx, proc_current\n\t"	\
	  "ljmp %0\n\t"				\
	  "1:"					\
	  ::"m"(*&_tmp.a), "m"(*&_tmp.b),	\
	  "d"(TSS(n)), "c"((long)proc[n]));	\
  }

#endif
