/*
 * Program: keyboard.c
 * Purpose: 键盘驱动程序
 * Author:  mofaph <mofaph@gmail.com>
 * Date:    2011-6-10
 */

#include <kernel/printk.h>      /* printk */
#include <asm/io.h>             /* inb */

void keyboard_init(void) {
}

void do_intr_keyboard(void) {
  unsigned char scan_code = inb(0x60); /* 读取扫描码 */
  printhex(scan_code);
  outb_p(0x20, 0x20);
}
