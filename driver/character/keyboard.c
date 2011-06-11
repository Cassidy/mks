/*
 * Program: keyboard.c
 * Purpose: 键盘驱动程序
 * Author:  mofaph <mofaph@gmail.com>
 * Date:    2011-6-10
 */

#include <kernel/printk.h>      /* printk */
#include <asm/io.h>             /* inb */
#include "keyboard.h"           /* keymap MAP_COL MAKE */

void keyboard_init(void) {
}

void do_intr_keyboard(void) {
  unsigned char scan_code = inb(0x60); /* 读取扫描码 */

  if ((scan_code & 0x80) == MAKE)         /* driver/character/keyboard.h */
    printchar(keymap[scan_code*MAP_COL]); /* MAP_COL=3 */

  /* 复位 8259A 的正在服务寄存器（ISR）。
     因为在 setup 中设置 8259A 工作在非自动结束方式，主芯片 IRQ2 连接从芯片，
     所以复位时要把 IRQ2 置位，其余的都复位。8259A 主芯片的端口地址是 0x20 */

  outb_p(0x20, 0x20);           /* outb_p(value, port) */
}
