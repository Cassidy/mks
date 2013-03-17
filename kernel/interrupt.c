/*********************************************
 * File name: interrupt.c
 * Author: Cassidy
 * Time-stamp: <2011-05-29 04:32:03>
 *********************************************
 */

#include <kernel/kernel.h>
#include <kernel/interrupt.h>
#include <kernel/head.h>
#include <asm/system.h>         /* set_trap_gate set_system_gate set_intr_gate */
#include <asm/io.h>             /* outb_p inb_p */

#define INTR_NUM 255

/* 定义两个同义的函数指针，指向返回类型是 void，参数也是 void 的函数 */
typedef void (*intr_addr_t)(void);
typedef void (*intr_proc_t)(void);

extern void do_intr_clock(long *, long, long);   /* kernel/time.c */
extern void do_intr_page(long *, unsigned long); /* kernel/memory.c */
extern void do_intr_keyboard(void);              /* driver/character/keyboard.c */

struct desc_struct *idt;              /* 定义指向中断描述符表的指针 */

intr_addr_t intr_enter[20] = {
  &intr0, &intr1, &intr2, &intr3, &intr4, &intr5, &intr6, &intr7,
  &intr8, &intr9, &intr10, &intr11, &intr12, &intr13, &intr14,
  &intr15, &intr16, &intr17, &intr18, &intr19
};
intr_addr_t hwintr_enter[16] = {
  &hwintr0, &hwintr1, &hwintr2, &hwintr3, &hwintr4, &hwintr5,
  &hwintr6, &hwintr7, &hwintr8, &hwintr9, &hwintr10, &hwintr11,
  &hwintr12, &hwintr13, &hwintr14, &hwintr15
};
intr_addr_t intr_reserv_enter  = &intr_reserved;
intr_addr_t intr_msg_enter     = &intr_msg;
intr_addr_t intr_kercall_enter = &intr_kercall;

intr_proc_t intr_table[INTR_NUM]; /* 中断处理函数表 */

void do_intr_parallel(void)
{
  outb(0x20, 0x20);
}

void do_intr_kercall(void)
{
  printk("kernel call");
}

void do_intr_reserved(void)
{
  printk("intr_reserved");
}

void do_intr_debug1(void)
{
  printk("intr_occur");
}

void do_intr_debug2(long esp, long error_code)
{
  printbin((unsigned long)error_code);
  println();
}


void do_intr_0(void){printk("intr_occur 0");}
void do_intr_1(void){printk("intr_occur 1");}
void do_intr_2(void){printk("intr_occur 2");}
void do_intr_3(void){printk("intr_occur 3");}
void do_intr_4(void){printk("intr_occur 4");}
void do_intr_5(void){printk("intr_occur 5");}
void do_intr_6(void){printk("intr_occur 6");}
void do_intr_7(void){printk("intr_occur 7");}
void do_intr_8(void){printk("intr_occur 8");}
void do_intr_9(void){printk("intr_occur 9");}
void do_intr_10(void){printk("intr_occur 10");}
void do_intr_11(void){printk("intr_occur 11");}
void do_intr_12(void){printk("intr_occur 12");}
void do_intr_13(void){printk("intr_occur 13");}
void do_intr_14(void){printk("intr_occur 14");}
void do_intr_15(void){printk("intr_occur 15");}
void do_intr_16(void){printk("intr_occur 16");}
void do_intr_17(void){printk("intr_occur 17");}
void do_intr_18(void){printk("intr_occur 18");}
void do_intr_19(void){printk("intr_occur 19");}
void do_intr_20(void){printk("intr_occur 20");}
void do_intr_21(void){printk("intr_occur 21");}
void do_intr_22(void){printk("intr_occur 22");}
void do_intr_23(void){printk("intr_occur 23");}
void do_intr_24(void){printk("intr_occur 24");}
void do_intr_25(void){printk("intr_occur 25");}
void do_intr_26(void){printk("intr_occur 26");}
void do_intr_27(void){printk("intr_occur 27");}
void do_intr_28(void){printk("intr_occur 28");}
void do_intr_29(void){printk("intr_occur 29");}
void do_intr_30(void){printk("intr_occur 30");}
void do_intr_31(void){printk("intr_occur 31");}
void do_intr_32(void){printk("intr_occur 32");}
void do_intr_33(void){printk("intr_occur 33");}
void do_intr_34(void){printk("intr_occur 34");}
void do_intr_35(void){printk("intr_occur 35");}
void do_intr_36(void){printk("intr_occur 36");}
void do_intr_37(void){printk("intr_occur 37");}
void do_intr_38(void){printk("intr_occur 38");}
void do_intr_39(void){printk("intr_occur 39");}
void do_intr_40(void){printk("intr_occur 40");}
void do_intr_41(void){printk("intr_occur 41");}
void do_intr_42(void){printk("intr_occur 42");}
void do_intr_43(void){printk("intr_occur 43");}
void do_intr_44(void){printk("intr_occur 44");}
void do_intr_45(void){printk("intr_occur 45");}
void do_intr_46(void){printk("intr_occur 46");}
void do_intr_47(void){printk("intr_occur 47");}
void do_intr_48(void){printk("intr_occur 48");}
void do_intr_49(void){printk("intr_occur 49");}
void do_intr_50(void){printk("intr_occur 50");}
void do_intr_51(void){printk("intr_occur 51");}
void do_intr_52(void){printk("intr_occur 52");}
void do_intr_53(void){printk("intr_occur 53");}
void do_intr_54(void){printk("intr_occur 54");}
void do_intr_55(void){printk("intr_occur 55");}
void do_intr_56(void){printk("intr_occur 56");}
void do_intr_57(void){printk("intr_occur 57");}
void do_intr_58(void){printk("intr_occur 58");}
void do_intr_59(void){printk("intr_occur 59");}

/* 中断初始化 */
void intr_init()
{
  int i;

  idt = IDT_ADDR;               /* IDT_ADDR = 0x6000 */

  /* 初始化所有中断处理函数为保留的 */
  for(i=0; i<255; i++)
    intr_table[i] = &do_intr_reserved;

  /* 设置特定的中断处理函数 */
  intr_table[13]   = &do_intr_debug2;
  intr_table[14]   = &do_intr_page;
  intr_table[32]   = &do_intr_clock;
  intr_table[33]   = &do_intr_keyboard;
  intr_table[39]   = &do_intr_parallel;
  intr_table[0x88] = &do_intr_kercall;

  intr_table[10]   = &do_intr_10;
  intr_table[11]   = &do_intr_11;
  intr_table[12]   = &do_intr_12;
  /* intr_table[13] = &do_intr_debug2; */
  /* intr_table[14] = &do_intr_page; */
  intr_table[15]   = &do_intr_15;
  intr_table[16]   = &do_intr_16;
  intr_table[17]   = &do_intr_17;
  intr_table[18]   = &do_intr_18;
  intr_table[19]   = &do_intr_19;
  intr_table[20]   = &do_intr_20;
  intr_table[21]   = &do_intr_21;
  intr_table[22]   = &do_intr_22;
  intr_table[23]   = &do_intr_23;
  intr_table[24]   = &do_intr_24;
  intr_table[25]   = &do_intr_25;
  intr_table[26]   = &do_intr_26;
  intr_table[27]   = &do_intr_27;
  intr_table[28]   = &do_intr_28;
  intr_table[29]   = &do_intr_29;
  intr_table[30]   = &do_intr_30;
  intr_table[31]   = &do_intr_31;
  /* intr_table[32] = &do_intr_clock; */
  /* intr_table[33]   = &do_intr_keyboard; */
  intr_table[34]   = &do_intr_34;
  intr_table[35]   = &do_intr_35;
  intr_table[36]   = &do_intr_36;
  intr_table[37]   = &do_intr_37;
  intr_table[38]   = &do_intr_38;
  /* intr_table[39] = &do_intr_parallel; */
  intr_table[40]   = &do_intr_40;
  intr_table[41]   = &do_intr_41;
  intr_table[42]   = &do_intr_42;
  intr_table[43]   = &do_intr_43;
  intr_table[44]   = &do_intr_44;
  intr_table[45]   = &do_intr_45;
  intr_table[46]   = &do_intr_46;
  intr_table[47]   = &do_intr_47;
  intr_table[48]   = &do_intr_48;
  intr_table[49]   = &do_intr_49;
  intr_table[50]   = &do_intr_50;
  intr_table[51]   = &do_intr_51;
  intr_table[52]   = &do_intr_52;
  intr_table[53]   = &do_intr_53;
  intr_table[54]   = &do_intr_54;
  intr_table[55]   = &do_intr_55;
  intr_table[56]   = &do_intr_56;
  intr_table[57]   = &do_intr_57;
  intr_table[58]   = &do_intr_58;
  intr_table[59]   = &do_intr_59;

  /* 设置中断描述符表 */
  for(i=0; i<=2; i++)
    set_trap_gate(i, intr_enter[i]);
  for(i=3; i<=5; i++)
    set_system_gate(i, intr_enter[i]);
  for(i=6; i<=19; i++)
    set_trap_gate(i, intr_enter[i]);
  for(i=20; i<=31; i++)
    set_trap_gate(i, intr_reserv_enter); /* 陷阱门，不复位 IF 标志 */
  for(i=32; i<=47; i++)
    set_trap_gate(i, hwintr_enter[i-32]);
  for(i=48; i<=255; i++)
    set_trap_gate(i, intr_reserv_enter);

  set_intr_gate(14, intr_enter[14]);         /* 页面错误，中断门 */
  set_system_gate(0x80, intr_msg_enter);     /* 消息中断 */
  set_system_gate(0x88, intr_kercall_enter); /* 内核调用中断 */

  /* 宏函数(include/asm/io.h)： outb_p(value, port) inb_p(port) */
  outb_p(inb_p(0x21) & 0xF9, 0x21); /* 开启键盘中断（IRQ1）和接连从芯片（IRQ2） */
}
