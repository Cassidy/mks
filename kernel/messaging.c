/*********************************************
 * File name: messaging.c
 * Author: Cassidy
 * Time-stamp: <2011-06-10 16:15:55>
 *********************************************
 */

#include <kernel/kernel.h>
#include <kernel/messaging.h>   /* msg_struct NR_MSG_HARDWARE */
#include <kernel/proc.h>

extern struct proc_struct * proc[NR_PROCS]; /* 所有进程指针 -- kernel/proc.c */
extern struct proc_struct * proc_current;   /* 当前进程指针 -- kernel/proc.c */

extern void schedule(void);     /* 进程调度 -- kernel/proc.c */
extern void share_page(unsigned long, unsigned long); /* kernel/memory.c */

struct msg_struct * small_send_head;
struct msg_struct * small_send_tail;
struct msg_struct * small_receive_head;
struct msg_struct * small_receive_tail;
struct msg_struct * big_send_head;
struct msg_struct * big_send_tail;
struct msg_struct * big_receive_head;
struct msg_struct * big_receive_tail;

/* NR_MSG_HARDWARE=1024 (include/kernel/messaging.h) */
struct msg_struct msg_hardware[NR_MSG_HARDWARE]; /* 硬件中断消息 */

/* 消息传递初始化 */
void msg_init(void)
{
  long i;
  
  small_send_head = small_send_tail = NULL;
  small_receive_head = small_receive_tail =  NULL;
  big_send_head = big_send_tail = NULL;
  big_receive_head = big_receive_tail = NULL;
 
  for(i=0; i<NR_MSG_HARDWARE; i++) {
    msg_hardware[i].src = 0;
    msg_hardware[i].dest = 0;
    msg_hardware[i].msg = 0;
    msg_hardware[i].next = NULL;
  }
}

/* 发送小消息处理 */
long small_send(long src, long * dest, long * msg)
{
  struct msg_struct * msg_p = small_receive_head;
  struct msg_struct * msg_t = NULL;

  if((src<0 && src!=MSG_HARDWARE) || src>=NR_PROCS)
    return -1;
  if(*dest < 0 || *dest >= NR_PROCS)
    return -1;

  while(msg_p)
    {
      if(*dest==msg_p->dest &&
	 (src==*((long *)(msg_p->src)) || *((long *)(msg_p->src))==MSG_ANY))
	break;
      msg_t = msg_p;
      msg_p = msg_p->next;
    }
  if(msg_p)
    {
      *((long *)(msg_p->msg)) = *msg;
      if(*((long *)(msg_p->src)) == MSG_ANY)
	*((long *)(msg_p->src)) = src;
      if(!msg_t)
        small_receive_head = msg_p->next;
      else
        msg_t->next = msg_p->next;
      if(!msg_p->next)
        small_receive_tail = msg_t;
      proc[msg_p->dest]->state = RUNNING;
      schedule();
      return 1;
    }

  msg_p = small_send_head;
  while(msg_p)
    {
      if(*dest==msg_p->src && src==msg_p->dest)
	return -1;
      msg_p = msg_p->next;
    }

  msg_p = big_send_head;
  while(msg_p)
    {
      if(*dest==msg_p->src && src==msg_p->dest)
	return -1;
      msg_p = msg_p->next;
    }

  msg_p = big_receive_head;
  while(msg_p)
    {
      if(*dest==msg_p->dest && src==*((long *)(msg_p->src)))
	return -1;
      msg_p = msg_p->next;
    }

  msg_p = NULL;
  if(src == MSG_HARDWARE)
    {
      int i;
      for(i=0; i<NR_MSG_HARDWARE; i++)
        if(msg_hardware[i].src != MSG_HARDWARE)
          {
            msg_p = &msg_hardware[i];
            break;
          }
      if(!msg_p)
        return -1;
    }
  else
    {
      struct msg_struct new_msg;
      msg_p = &new_msg;
    }

  msg_p->src = src;
  msg_p->dest = *dest;
  msg_p->msg = *msg;
  msg_p->next = NULL;

  if(!small_send_head)
    small_send_head = small_send_tail = msg_p;
  else
    {
      small_send_tail->next = msg_p;
      small_send_tail = msg_p;
    }

  if(src != MSG_HARDWARE)
    proc[src]->state = SLEEPING;
  schedule();
  return 1;
}

/* 接收小消息处理 */
long small_receive(long * src, long dest, long * msg)
{
  struct msg_struct * msg_p = small_send_head;
  struct msg_struct * msg_t = NULL;

  if((*src<0 && *src!=MSG_ANY && *src!=MSG_HARDWARE) || *src>=NR_PROCS)
    return -1;
  if(dest < 0 || dest >= NR_PROCS)
    return -1;

  while(msg_p)
    {
      if(dest==msg_p->dest && (*src==msg_p->src || *src==MSG_ANY))
	break;
      msg_t = msg_p;
      msg_p = msg_p->next;
    }
  if(msg_p)
    {
      *msg = msg_p->msg;
      if(*src == MSG_ANY)
	*src = msg_p->src;
      if(!msg_t)
        small_send_head = msg_p->next;
      else
        msg_t->next = msg_p->next;
      if(!msg_p->next)
        small_send_tail = msg_t;
      if(msg_p->src == MSG_HARDWARE)
        msg_p->src = 0;
      else
        proc[msg_p->src]->state = RUNNING;
      schedule();
      return 1;
    }

  msg_p = small_receive_head;
  while(msg_p)
    {
      if(dest==*((long *)(msg_p->src)) && *src==msg_p->dest)
	return -1;
      msg_p = msg_p->next;
    }

  msg_p = big_send_head;
  while(msg_p)
    {
      if(dest==msg_p->dest && *src==msg_p->src)
	return -1;
      msg_p = msg_p->next;
    }

  msg_p = big_receive_head;
  while(msg_p)
    {
      if(dest==*((long *)(msg_p->src)) && *src==msg_p->dest)
	return -1;
      msg_p = msg_p->next;
    }

  struct msg_struct new_msg;
  msg_p = &new_msg;
  msg_p->src = (long)src;
  msg_p->dest = dest;
  msg_p->msg = (long)msg;
  msg_p->next = NULL;

  if(!small_receive_head)
    small_receive_head = small_receive_tail = msg_p;
  else
    {
      small_receive_tail->next = msg_p;
      small_receive_tail = msg_p;
    }

  proc[dest]->state = SLEEPING;
  schedule();
  return 1;
}

/* 发送大消息处理 */
long big_send(long src, long * dest, long * msg)
{
  struct msg_struct * msg_p = big_receive_head;
  struct msg_struct * msg_t = NULL;

  if(src < 0 || src >= NR_PROCS)
    return -1;
  if(*dest < 0 || *dest >= NR_PROCS)
    return -1;

  while(msg_p)
    {
      if(*dest==msg_p->dest &&
	 (src==*((long *)(msg_p->src)) || *((long *)(msg_p->src))==MSG_ANY))
	break;
      msg_t = msg_p;
      msg_p = msg_p->next;
    }
  if(msg_p)
    {
      share_page((unsigned long)(*msg), (unsigned long)(msg_p->msg));
      if(*((long *)(msg_p->src)) == MSG_ANY)
	*((long *)(msg_p->src)) = src;
      if(!msg_t)
        big_receive_head = msg_p->next;
      else
        msg_t->next = msg_p->next;
      if(!msg_p->next)
        big_receive_tail = msg_t;
      proc[msg_p->dest]->state = RUNNING;
      schedule();
      return 1;
    }

  msg_p = small_send_head;
  while(msg_p)
    {
      if(dest==msg_p->src && src==msg_p->dest)
        return -1;
      msg_p = msg_p->next;
    }

  msg_p = small_receive_head;
  while(msg_p)
    {
      if(dest==msg_p->dest && src==*((long *)(msg_p->src)))
	return -1;
      msg_p = msg_p->next;
    }

  msg_p = big_send_head;
  while(msg_p)
    {
      if(dest==msg_p->src && src==msg_p->dest)
        return -1;
      msg_p = msg_p->next;
    }

  struct msg_struct new_msg;
  msg_p = &new_msg;
  msg_p->src = src;
  msg_p->dest = *dest;
  msg_p->msg = *msg;
  msg_p->next = NULL;

  if(!big_send_head)
    big_send_head = big_send_tail = msg_p;
  else
    {
      big_send_tail->next = msg_p;
      big_send_tail = msg_p;
    }

  proc[src]->state = SLEEPING;
  schedule();
  return 1;
}

/* 接收大消息处理 */
long big_receive(long * src, long dest, long * msg)
{
  struct msg_struct * msg_p = big_send_head;
  struct msg_struct * msg_t = NULL;

  if((*src<0 && *src!=MSG_ANY) || *src>=NR_PROCS)
    return -1;
  if(dest < 0 || dest >= NR_PROCS)
    return -1;

  while(msg_p)
    {
      if(dest==msg_p->dest && (*src==msg_p->src || *src==MSG_ANY))
	break;
      msg_t = msg_p;
      msg_p = msg_p->next;
    }
  if(msg_p)
    {
      share_page((unsigned long)(msg_p->msg), (unsigned long)(*msg));
      if(*src == MSG_ANY)
	*src = msg_p->src;
      if(!msg_t)
        big_send_head = msg_p->next;
      else
        msg_t->next = msg_p->next;
      if(!msg_p->next)
        big_send_tail = msg_t;
      proc[msg_p->src]->state = RUNNING;
      schedule();
      return 1;
    }

  msg_p = small_send_head;
  while(msg_p)
    {
      if(dest==msg_p->dest && *src==msg_p->src)
	return -1;
      msg_p = msg_p->next;
    }

  msg_p = small_receive_head;
  while(msg_p)
    {
      if(dest==*((long *)(msg_p->src)) && *src==msg_p->dest)
	return -1;
      msg_p = msg_p->next;
    }

  msg_p = big_receive_head;
  while(msg_p)
    {
      if(dest==*((long *)(msg_p->src)) && *src==msg_p->dest)
	return -1;
      msg_p = msg_p->next;
    }

  struct msg_struct new_msg;
  msg_p = &new_msg;
  msg_p->src = (long)src;
  msg_p->dest = dest;
  msg_p->msg = *msg;
  msg_p->next = NULL;

  if(!big_receive_head)
    big_receive_head = big_receive_tail = msg_p;
  else
    {
      big_receive_tail->next = msg_p;
      big_receive_tail = msg_p;
    }

  proc[dest]->state = SLEEPING;
  schedule();
  return 1;
}

/* 消息中断处理函数 */
long do_intr_msg(long function, long * src_dest, long * msg)
{
  long a;
  long cur_pid = proc_current->pid;
  long entry;
  if(cur_pid == *src_dest)
    return -1;   		/* 消息传递失败 */
  if(function == 1)
    a = small_send(cur_pid, src_dest, msg);
  else if(function == 2)
    a = small_receive(src_dest, cur_pid, msg);
  else if(function == 3)
    {
      entry = *msg;
      entry = (entry & 0xFFFFF000) + 0x1000;
      /* 消息线性地址 ＝ 消息偏移地址 ＋ 段基地址*/
      entry += cur_pid * 0x4000000;
      a = big_send(cur_pid, src_dest, &entry);
    }
  else if(function == 4)
    {
      entry = *msg;
      entry = (entry & 0xFFFFF000) + 0x1000;
      /* 消息线性地址 ＝ 消息偏移地址 ＋ 段基地址*/
      entry += cur_pid * 0x4000000;
      a = big_receive(src_dest, cur_pid, &entry);
    }
  else
    return -1;     		/* 消息传递失败 */
  return a;
}

