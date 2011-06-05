/*********************************************
 * File name: time.c
 * Author: Cassidy
 * Time-stamp: <2011-05-29 20:13:38>
 *********************************************
 */

#include <kernel/kernel.h>
#include <asm/io.h>
#include <kernel/time.h>        /* tm */
#include <kernel/proc.h>        /* proc_struct */

/* outb_p(value, port) inb_p(port) -- include/asm/io.h

   CMOS 的地址空间在基本地址空间之外，因此其中不包括可执行代码。要访问它需要通过
   端口 0x70、0x71 进行。0x70 是地址端口，0x71 是数据端口。为了读取指定
   偏移位置的字节，必须首先使用 out 指令向地址端口 0x70 发送指定字节的偏移
   位置值，然后使用 in 指令从数据端口 0x71 读取指定的字节信息。同样，对于写操作
   也需要首先向地址端口 0x70 发送指定字节的偏移值，然后把数据写到数据端口 0x71。 */
#define CMOS_READ(addr) ({ \
      outb_p(0x80 | addr, 0x70); \
      inb_p(0x71); \
    })

/* BCD 码利用半个字节（4位）表示一个十进制数，因此一个字节表示两个十进制数。
   (val)&15 取 BCD 表示的十进制个位数，而 (val)>>4 取 BCD 表示的十进制
   十位数，再乘以 10。因此最后两者相加就是一个字节 BCD 码的实际二进制数值。 */
#define BCD_TO_BIN(val) ((val) = ((val)&15)+((val)>>4)*10)

#define MINUTE 60
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)
#define YEAR (365*DAY)

long volatile jiffies;
long startup_time = 0;

extern void schedule(void);               /* 进程调度(kernel/proc.c) */
extern struct proc_struct * proc_current; /* 当前进程指针(include/kernel/proc.h) */

/* interestingly, we assume leap-years */
static int month[12] = {
	0,
	DAY*(31),
	DAY*(31+29),
	DAY*(31+29+31),
	DAY*(31+29+31+30),
	DAY*(31+29+31+30+31),
	DAY*(31+29+31+30+31+30),
	DAY*(31+29+31+30+31+30+31),
	DAY*(31+29+31+30+31+30+31+31),
	DAY*(31+29+31+30+31+30+31+31+30),
	DAY*(31+29+31+30+31+30+31+31+30+31),
	DAY*(31+29+31+30+31+30+31+31+30+31+30)
};

/* kernel_mktime: 计算开机时间 */
long kernel_mktime(struct tm * tm)
{
	long res;
	int year;                   /* 从 UNIX 诞生（1970）到现在经过的时间 */

    /* 因为是两位表示方式，会有 2000年 问题。可以加上 100 暂时解决这个问题。
       但是，到了 2100 年，这段程序如果还在使用，就得重写。 */
	if(tm->tm_year < 70) tm->tm_year += 100;

	year = tm->tm_year - 70;
/* magic offsets (y+1) needed to get leapyears right.*/
	res = YEAR*year + DAY*((year+1)/4);
	res += month[tm->tm_mon];
/* and (y+2) here. If it wasn't a leap-year, we have to adjust */
	if (tm->tm_mon>1 && ((year+2)%4))
		res -= DAY;
	res += DAY*(tm->tm_mday-1);
	res += HOUR*tm->tm_hour;
	res += MINUTE*tm->tm_min;
	res += tm->tm_sec;

	return res;
}

/* time_init: 时间初始化 */
void time_init(void)
{
  struct tm time;               /* tm -- include/kernel/time.h */

  do {
    time.tm_sec = CMOS_READ(0);
    time.tm_min = CMOS_READ(2);
    time.tm_hour = CMOS_READ(4);
    time.tm_mday = CMOS_READ(7);
    time.tm_mon = CMOS_READ(8);
    time.tm_year = CMOS_READ(9);
  } while (time.tm_sec != CMOS_READ(0));

  BCD_TO_BIN(time.tm_sec);
  BCD_TO_BIN(time.tm_min);
  BCD_TO_BIN(time.tm_hour);
  BCD_TO_BIN(time.tm_mday);
  BCD_TO_BIN(time.tm_mon);
  BCD_TO_BIN(time.tm_year);
  time.tm_mon--;                /* tm_mon 中月份范围是 0~11 */
  startup_time = kernel_mktime(&time);
  jiffies = 0;

  /* 宏函数 outb_p(value, port) inb_p(port) -- include/asm/io.h */
  outb_p(0x36, 0x43);
  outb_p(LATCH & 0xFF, 0x40);   /* LATCH -- include/config.h */
  outb_p(LATCH >> 8, 0x40);
  outb(inb_p(0x21)&~0x01, 0x21);
}

/* do_intr_clock: 时钟中断处理程序 */
void do_intr_clock(long *eip, long error_code, long cpl)
{
  printa(proc_current->pid);
  println();
  jiffies++;
  
  /* 由于初始化中断控制芯片时没有采用自动EOI,所以这里需要发指令结束该硬件中断 */
  if (jiffies < 15)
    outb(0x20, 0x20);

  if (cpl != 0)
    proc_current->utime++;
  else /* cpl == 0 */
    proc_current->stime++;

  if((--(proc_current->counter)) > 0)
    return;

  proc_current->counter = 0;

  if(cpl == 0)
    return;

  schedule();
}
