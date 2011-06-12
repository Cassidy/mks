/*********************************************
 * File name: printk.c
 * Author: Cassidy
 * Time-stamp: <2011-05-10 21:30:23>
 *********************************************
 */

#include <asm/io.h>     /* outb_p */

#define ORIG  0xb8000                        /* 显存首地址 */
static unsigned long video_num_columns = 80; /* 屏幕文本列数 */
static unsigned long pos;                    /* 显示内存一位置 */
static unsigned long x,y;                    /* 当前光标位置 */
static unsigned char attr = 0x07;            /* 字符属性(黑底白字) */

static inline void update_cursor(void);
static inline void write_char(char);
void con_init(void);                       /* 显示初始化 */
int printk(const char *);                  /* 输出字符串 */
int prints(const char *);                  /* 输出字符串 */
void printa(const long a);                 /* 输出长整型变量 */
void printbin(const unsigned long);        /* 输出二进制数 */
void printchar(unsigned char c);           /* 显示字符 */
void printhex(unsigned char c);            /* 显示十六进制数 */


/* 显示初始化 */
void con_init()
{
  x = 0;
  y = 0;
  char * ph = ORIG;             /* 显存开始位置 */
  char * pe = ORIG + 4000;      /* 显存结束位置 */

  /* 清屏 */
  while(ph < pe) {
    *ph++ = 32;                 /* 显示空字符 */
    *ph++ = attr;               /* 黑底白字 */
  }

  printk("                  MKS -- Micro Kernel for Study                   ");

  update_cursor();
}

/* 输出一字符,光标位置进一 */
static inline void write_char(char ch)
{
  pos = ORIG + video_num_columns * 2 * y + x * 2;
  *((char *)pos) = ch;
  *((char *)(pos+1)) = attr;
  x++;

  if(x >= 80) {
      y += x / 80;
      x = x % 80;
  }

  if(y >= 25) {
    con_init();
    y = y % 25;
  }
}

/* update_cursor: 设置光标跟随，更新光标位置 */
static inline void update_cursor(void) {
  pos = ORIG + (video_num_columns * y + x) * 2;

  outb_p(0xE, 0x3D4);
  outb_p((pos>>8) & 0xFF, 0x3D5);
  outb_p(0xF, 0x3D4);
  outb_p(pos & 0xFF, 0x3D5);
}

/* 光标移至下一行开头 */
void println(void)
{
  char * ph = ORIG + video_num_columns * 2 * y + x * 2; //原光标位置
  char * pe;
  x = 0;
  y++;

  if(y < 25)
    pe = ORIG + video_num_columns * 2 * (y + 1);
  else {
    /* 先清除屏幕的第一行 */
    char * ph1 = ORIG;
    char * pe1 = ORIG + video_num_columns * 2;

    while(ph1 < pe1) {
	  *ph1++ = 32;
	  *ph1++ = attr;
	}

    pe = ORIG + video_num_columns * 2 * y;
    y = 0;
  }

  while(ph < pe) {
    *ph++ = 32;
    *ph++ = attr;
  }
}

/* 输出字符串,返回输出字符个数 */
int prints(const char *fmt)
{
  int i;
  char * ps = fmt;

  for(i=0; *ps!='\0'; i++) {
    write_char(*ps);
    ps++;
  }

  return i;
}

/* 将十进制数每位转为字符输出 */
static inline void print_charina(unsigned long a)
{
  unsigned long b, c;
  if(a == 0)
    return;
  b = a / 10;
  c = b * 10;
  c = a - c;
  print_charina(b);
  c = c + 48;
  write_char((char)c);
}

/* 输出长整型娈量 */
void printa(const long a)
{
  unsigned long b = (unsigned long)a;
  unsigned long c = b >> 31;
  if(c == 1)
    {
      write_char('-');
      b = ~(b-1);
    }
  c = b & 0x7FFFFFFF;
  if(c == 0)
      write_char(48);
  else
    print_charina(c);
}

/* 输出二进制数 */
void printbin(const unsigned long el)
{
  int i;
  unsigned long ef = el;

  for(i=32; i>0; i--) {
    if(ef & 0x80000000)
      write_char(49);
    else
      write_char(48);
    ef = ef << 1;
  }
}

/* printhex: 输出十六进制数 */
void printhex(unsigned char c) {
  int i;
  unsigned char tmp;

  write_char(48);               /* '0' */
  write_char(120);              /* 'x' */

  for (i = 0; i < 2; ++i) {
    c = (c << 4) | (c >> 4);    /* 右旋 4 位 */
    tmp = c & 0x0F;

    if (tmp > 9)
      tmp = tmp - 10 + 'A';
    else if (0 <= tmp && tmp <= 9)
      tmp = tmp + '0';
    else /* error */
      printk("error");

    write_char(tmp);
  }
}

/* printchar: 打印字符，供其他程序调用 */
void printchar(unsigned char c) {
  pos = ORIG + (video_num_columns * y + x) * 2;
  (*(char *)pos)       = c;
  (*(char *)(pos + 1)) = attr;
  ++x;

  if (x >= 80) {
    x = 0;
    ++y;
  }

  if (y >= 25) {
    y = 1;
    x = 0;
  }
}

/* 内核输出函数,暂时使用prints(输出字符串) */
int printk(const char *fmt)
{
  int a;
  a = prints(fmt);
  println();
  return a;
}
