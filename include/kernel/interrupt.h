/*********************************************
 * File name: interrupt.h
 * Author: Cassidy
 * Time-stamp: <2011-04-18 23:39:49>
 *********************************************
 */

#ifndef _INTERRUPT_H
#define _INTERRUPT_H

/* 下面的函数在 kernel/asm.s中定义 */
extern void intr0();
extern void intr1();
extern void intr2();
extern void intr3();
extern void intr4();
extern void intr5();
extern void intr6();
extern void intr7();
extern void intr8();
extern void intr9();
extern void intr10();
extern void intr11();
extern void intr12();
extern void intr13();
extern void intr14();
extern void intr15();
extern void intr16();
extern void intr17();
extern void intr18();
extern void intr19();

extern void hwintr0();
extern void hwintr1();
extern void hwintr2();
extern void hwintr3();
extern void hwintr4();
extern void hwintr5();
extern void hwintr6();
extern void hwintr7();
extern void hwintr8();
extern void hwintr9();
extern void hwintr10();
extern void hwintr11();
extern void hwintr12();
extern void hwintr13();
extern void hwintr14();
extern void hwintr15();
extern void hwintr16();

extern void intr_msg();
extern void intr_kercall();
extern void intr_reserved();

#endif
