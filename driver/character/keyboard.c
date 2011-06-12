/*
 * Program: keyboard.c
 * Purpose: 键盘驱动程序
 * Author:  mofaph <mofaph@gmail.com>
 * Date:    2011-6-10
 */

#include <kernel/printk.h>      /* printk */
#include <asm/io.h>             /* inb */
#include "keyboard.h"           /* keymap kbdqueue MAP_COL MAKE */

static struct kbdqueue kbd;     /* 键盘缓冲区队列 */

static int enqueue(unsigned char);    /* 插入队列 */
static void dequeue(void);            /* 出队列 */
static unsigned char headqueue(void); /* 查看队列头 */

/* 键盘中断已在 interrupt.c 中开启了 */
void keyboard_init(void) {
  kbd.head = kbd.tail = kbd.buf;
}

void do_intr_keyboard(void) {
  unsigned char keycode;
  unsigned char keymapcol = NO_SHIFT;  /* NO_SHIFT = 0 */
  unsigned char scan_code = inb(0x60); /* 读取扫描码 */

  if ((scan_code & 0x80) == MAKE) {
    if ((scan_code != SHIFT_L) && (scan_code != SHIFT_R)) {
      if ((keycode = headqueue()) == SHIFT_L || keycode == SHIFT_R)
        keymapcol = WITH_SHIFT;
      printchar(keymap[scan_code*MAP_COL+keymapcol]); /* MAP_COL=3 */
    } else if ((scan_code == SHIFT_L) || (scan_code == SHIFT_R)) {
      enqueue(scan_code);
    } else
      /* do nothing */;
  } else if ((scan_code & 0x80) == BREAK) {
    if (((scan_code & 0x7F) == SHIFT_L) || ((scan_code & 0x7F) == SHIFT_R))
      dequeue();
  } else
    /* do nothing */;

  /* 复位 8259A 的正在服务寄存器（ISR）。
     因为在 setup 中设置 8259A 工作在非自动结束方式，主芯片 IRQ2 连接从芯片，
     所以复位时要把 IRQ2 置位，其余的都复位。8259A 主芯片的端口地址是 0x20 */

  outb_p(0x20, 0x20);           /* outb_p(value, port) */
}

/* enqueue: 插入键盘缓冲区队列 */
static int enqueue(unsigned char code) {
  /* 键盘缓冲区队列已满，则不能插入 */
  if (kbd.tail == kbd.buf + KBD_BUF_SIZE)
    return -1;

  *kbd.tail++ = code;
  return 0;
}

/* dequeue: 队列为空时返回；队列非空时，返回队列头元素 */
static void dequeue(void) {
  if (kbd.tail == kbd.buf)
    return;

  if (kbd.head == kbd.tail && kbd.head != kbd.buf) {
    kbd.head = kbd.tail = kbd.buf;
    return;
  }

  kbd.head++;
}

/* headqueue: 查看队列头；队列为空时，返回 0 */
static unsigned char headqueue(void) {
  if (kbd.tail == kbd.head)
    return 0;
  return *kbd.head;
}
