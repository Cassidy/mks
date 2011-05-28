/*********************************************
 * File name: main.c
 * Author: Cassidy
 * Time-stamp: <2011-05-29 00:37:41>
 *********************************************
 */

#include <kernel/kernel.h>
#include <asm/system.h>

#define RAM_SIZE (*(unsigned long *)0x90000) /* 内存总量，以字节为单位 */

extern void mem_init(unsigned long start, unsigned long end);     //内存初始化
extern void intr_init(void);                    //中断初始化
extern void time_init(void);                    //时间初始化
extern void proc_init(void);                    //进程初始化
extern void msg_init(void);                     //消息传递初始化

/* 物理地址应该不可能出现负数 */
static unsigned long memory_end = 0;             //机器具有的物理内存容量(字节数)
static unsigned long buffer_memory_end = 0;      //高速缓冲区末端地址
static unsigned long main_memory_start = 0;      //主内存(将用于分页)开始的位置

void main(void)
{
  con_init();              //显示初始化

  memory_end = RAM_SIZE;		  /* 内存总量（B） */
  memory_end &= 0xFFFFF000;                  //忽略不到4KB(1页)的内存数

  if(memory_end > 64*1024*1024)              //如果内存量超过64MB,则按64MB计
    memory_end = 64*1024*1024;

  if(memory_end > 12*1024*1024)              //如果内存>12MB,则设置缓冲区末端=4MB
    buffer_memory_end = 4*1024*1024;
  else if(memory_end > 6*1024*1024)          //否则若内存>6MB,则设置缓冲区末端=2MB
    buffer_memory_end = 2*1024*1024;
  else /* 1M < memory_end && memory_end <= 6M */
    buffer_memory_end = 1*1024*1024;        //否则设置缓冲区末端=1MB

  main_memory_start = buffer_memory_end;    //主内存起始位置=缓冲区末端

  mem_init(main_memory_start, memory_end);   //主内存初始化(mm/memory.c)
  intr_init();    //中断初始化(kernel/interrupt.c)
  time_init();    //时间初始化(kernel/time.c)
  proc_init();    //进程初始化(kernel/proc.c)
  msg_init();     //消息传递初始化(kernel/messaging.c)

  sti();
  move_to_user_mode();
  idle();
}

/*idle进程入口*/
void idle(void)
{
  for(;;)
    ;
}

