/*********************************************
 * File name: proc.c
 * Author: Cassidy
 * Time-stamp: <2011-05-29 20:50:40>
 *********************************************
 */

#include <kernel/kernel.h>      /* PAGE_SIZE */
#include <kernel/proc.h>        /* proc_struct INIT_PROC_DATA ltr lldt FIRST_TSS_ENTRY */
#include <asm/system.h>         /* set_tss_desc set_ldt_desc set_ldt_cs_desc set_ldt_ds_desc */

struct desc_struct *gdt;        /* 全局描述符表的入口地址 */

union proc_union {              /* 进程控制体与其内核态堆栈,总共4KB */
  struct proc_struct proc;      /* proc_struct -- include/kernel/proc.h */
  char stack[PAGE_SIZE];        /* PAGE_SIZE=4096 (include/kernel/kernel.h) */
};

/* NR_INIT_PROCS=3, INIT_PROC_DATA (include/config.h) */
union proc_union init_procs[NR_INIT_PROCS] = {
  {INIT_PROC_DATA,},
};

/* NR_PROCS=64(include/config.h) */
struct proc_struct * proc[NR_PROCS]; /* 所有进程指针 */

struct proc_struct * task_head;    /* 任务队列头指针 */
struct proc_struct * task_tail;    /* 任务队列尾指针 */
struct proc_struct * server_head;  /* 服务器队列头指针 */
struct proc_struct * server_tail;  /* 服务器队列尾指针 */
struct proc_struct * user_head;    /* 用户进程队列头指针 */
struct proc_struct * user_tail;    /* 用户进程队列尾指针 */
struct proc_struct * proc_current; /* 当前进程指针 */

extern void idle(void);         /* idle进程入口 (kernel/main.c) */
extern void init_proc(void);    /* init进程入口 (kernel/init_proc.c) */
extern void system_task(void);  /* system进程入口 (system_task/system.c) */
extern int share_multi_pages(unsigned long from, unsigned long to, long amount); /* kernel/memory.c */

/* 进程初始化 */
void proc_init(void)
{
  int i,j;
  unsigned long *pp, *qq;
  struct desc_struct * p;       /* desc_struct -- include/kernel/head.h */

  gdt = GDT_ADDR;               /* GDT_ADDR = 0x6800 (include/config.h) */
  for(i=0; i<NR_PROCS; i++)     /* NR_PROCS = 64 (include/config) */
    proc[i] = NULL;
  p = gdt + FIRST_TSS_ENTRY;    /* FIRST_TSS_ENTRY=4 (include/kernel/proc.h) */

  /* 初始化进程的局部描述符和状态描述符 */
  for(i=0; i<NR_PROCS*2; i++,p++)
    p->a = p->b = 0;

  /* 初始化 3 个进程，进程号依次是 0 1 2 */
  for(i=0; i<NR_INIT_PROCS; i++) { /* NR_INIT_PROCS=3 (include/config.h) */
    proc[i] = (struct proc_struct *)&(init_procs[i]);

    /*******************************************/
    /* 不知为什么,使用不了*proc[i] = *proc[0]方式赋值。使用下面的语句实现 */
    pp = (unsigned long *)proc[i];
    qq = (unsigned long *)proc[0];
    for(j=0; j<(PAGE_SIZE>>2); j++)
      *(pp++) = *(qq++);
    /*******************************************/

    proc[i]->pid = i;
    proc[i]->tss.esp0 = PAGE_SIZE + (long)proc[i]; /* PAGE_SIZE=4096 -- include/kernel/kernel.h */
    proc[i]->tss.ldt = LDT(i);  /* LDT(n) -- include/kernel/proc.h */

    proc[i]->tss.esp = 0x4000000; /* 用户堆栈指针,指向64M末 */
    proc[i]->tss.ebp = proc[i]->tss.esp;

    /* 以下四个宏函数在 include/asm/system.h 里定义 */
    set_tss_desc(gdt + FIRST_TSS_ENTRY + i*2, &(proc[i]->tss));
    set_ldt_desc(gdt + FIRST_LDT_ENTRY + i*2, &(proc[i]->ldt));      
    set_ldt_cs_desc((long *)&(proc[i]->ldt[1]), 0x4000000*i, 16384); /* limit=64M/4K=16*1024=16384 */
    set_ldt_ds_desc((long *)&(proc[i]->ldt[2]), 0x4000000*i, 16384);
      
    /* 共享多个物理页面, 线性地址分别分 0 和 0x4000000*i, 共享页面个数为 640KB/4KB=160 */
    if(i != 0)    share_multi_pages(0, 0x4000000*i, 160);
  }

  PROC_INIT;                    /* include/config.h */

  task_head = task_tail = NULL;
  server_head = server_tail = NULL;
  user_head = user_tail = NULL;
  proc_current = proc[PROC_IDLE_NR]; /* PROC_IDLE_NR=0 (include/config.h) */
  
  __asm__("pushfl; andl $0xFFFFBFFF, (%esp); popfl"); /* 任务嵌套标志置零 */
  ltr(PROC_IDLE_NR);            /* ltr(n)  -- include/kernel/proc.h */
  lldt(PROC_IDLE_NR);           /* lldt(n) -- include/kernel/proc.h */
}

/* 增加可运行进程到等待队列 */
void add_running_proc(void)
{
  int i;
  int flag = 0;
  struct proc_struct ** proc_head;
  struct proc_struct ** proc_tail;
  struct proc_struct * proc_temp;

  for(i=1; i<NR_PROCS; i++) {
    if(proc[i] && (proc[i]->state==RUNNING) && proc[i]!=proc_current) {
      if(proc[i]->proc_type == 1) {
        proc_head = &task_head;
        proc_tail = &task_tail;
      } else if(proc[i]->proc_type == 2) {
        proc_head = &server_head;
        proc_tail = &server_tail;
      } else if(proc[i]->proc_type == 3) {
        proc_head = &user_head;
        proc_tail = &user_tail;
      }

      proc_temp = *proc_head;
      if(!proc_temp) {
        *proc_head = *proc_tail = proc[i];
        proc[i]->next_ready = NULL;
        continue;
      }

      while(proc_temp) {
        if(proc_temp->pid == i) {
          flag = 1;
          break;
        }
        proc_temp = proc_temp->next_ready;
      }

      if(flag == 0) {
        (*proc_tail)->next_ready = proc[i];
        *proc_tail = proc[i];
        proc[i]->next_ready = NULL;
      }
      flag = 0;
    }
  }
}

/* 进程调度函数 */
void schedule(void)
{
  int next = 0;
  int current_type = proc_current->proc_type;
  int flag = current_type;
  int counter = COUNTER;     /* 时间片大小 */

  add_running_proc();   /* 增加可运行进程到等待队列 */

  if(proc_current->state != RUNNING)
    flag = 3;

  /* 选出一个合适的进程准备运行 */
  if(task_head) {
    next = task_head->pid;
    task_head = task_head->next_ready;
    if(!task_head)
      task_tail = NULL;
  } else if(server_head && (flag>1)) {
    next = server_head->pid;
    server_head = server_head->next_ready;
    if(!server_head)
      server_tail = NULL;
  } else if(user_head && (flag>2)) {
    next = user_head->pid;
    user_head = user_head->next_ready;
    if(!user_head)
      user_tail = NULL;
  }

  /* 如果没有其它进程可选且当前进程是running状态,则返回 */
  if((next==0) && (proc_current->state==RUNNING)) {
    if(proc_current->pid != 0)
      proc_current->counter = counter;
    return;
  }
  /* 如果没有其它进程可选且当前进程不是running状态,则切换到idle进程 */
  else if((next==0) && (proc_current->state!=RUNNING))
    counter = 0;
  /* 如果当前进程是running状态,将它插入就绪队列 */
  else if((next!=0) && (proc_current->state==RUNNING) && (proc_current->pid!=0)) {
    if(current_type==1) {
      if(!task_head)
        task_head = task_tail = proc_current;
      else {
        task_tail->next_ready = proc_current;
        task_tail = proc_current;
        task_head = task_head->next_ready;
      }
    } else if(current_type==2) {
      if(!server_head)
        server_head = server_tail = proc_current;
      else {
        server_tail->next_ready = proc_current;
        server_tail = proc_current;
        server_head = server_head->next_ready;
      }
    } else if(current_type==3) {
      if(!user_head)
        user_head = user_tail = proc_current;
      else {
        user_tail->next_ready = proc_current;
        user_tail = proc_current;
        user_head = user_head->next_ready;
      }
    }
    proc_current->next_ready = NULL;
  }

  proc[next]->counter = counter;
  proc[next]->next_ready = NULL;
  switch_to(next);   /* 切换到下一个进程 */
}
