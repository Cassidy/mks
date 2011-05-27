/*********************************************
 * File name: system.h
 * Author: Cassidy
 * Time-stamp: <2011-05-09 23:06:36>
 *********************************************
 */

#ifndef _SYSTEM_H
#define _SYSTEM_H

#define move_to_user_mode()			\
  __asm__("movl %%esp,%%eax\n\t"		\
	  "pushl $0x17\n\t"			\
	  "pushl %%eax\n\t"			\
	  "pushfl\n\t"			     	\
	  "pushl $0x0F\n\t"			\
	  "pushl $1f\n\t"			\
	  "iret\n\t"				\
	  "1:\tmovl $0x17, %%eax\n\t"		\
	  "movw %%ax, %%ds\n\t"			\
	  "movw %%ax, %%es\n\t"			\
	  "movw %%ax, %%fs\n\t"			\
	  "movw %%ax, %%gs"			\
	  :::"ax")

#define sti() __asm__ ("sti"::)
#define cli() __asm__ ("cli"::)
#define nop() __asm__ ("nop"::)

#define iret() __asm__ ("iret"::)

#define _set_gate(gate_addr,type,dpl,addr) \
__asm__ ("movw %%dx,%%ax\n\t" \
	"movw %0,%%dx\n\t" \
	"movl %%eax,%1\n\t" \
	"movl %%edx,%2" \
	: \
	: "i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
	"o" (*((char *) (gate_addr))), \
	"o" (*(4+(char *) (gate_addr))), \
	"d" ((char *) (addr)),"a" (0x00080000))

#define set_intr_gate(n,addr) \
  _set_gate(idt + n, 14, 0, addr)

#define set_trap_gate(n,addr) \
	_set_gate(idt + n,15,0,addr)

#define set_system_gate(n,addr) \
	_set_gate(idt + n,15,3,addr)


#define _set_ldt_seg_desc(gate_addr,type,base,limit) {	\
    *(gate_addr) = (((base) & 0x0000ffff)<<16) |	\
      ((limit) & 0x0ffff);				\
    *((gate_addr)+1) = ((base) & 0xff000000) |		\
      (((base) & 0x00ff0000)>>16) |			\
      ((limit) & 0xf0000) |				\
      (0x00C0F000) |					\
      ((type)<<8); }

#define set_ldt_cs_desc(gate_addr,base,limit)	\
  _set_ldt_seg_desc(gate_addr,0x0A,base,limit)
#define set_ldt_ds_desc(gate_addr,base,limit)	\
  _set_ldt_seg_desc(gate_addr,0x02,base,limit)

#define _set_tssldt_desc(n,addr,type) \
__asm__ ("movw $104,%1\n\t" \
	"movw %%ax,%2\n\t" \
	"rorl $16,%%eax\n\t" \
	"movb %%al,%3\n\t" \
	"movb $"type",%4\n\t"\
	"movb $0x00,%5\n\t" \
	"movb %%ah,%6\n\t" \
	"rorl $16,%%eax" \
	 ::"a" (addr), "m" (*(n)), "m" (*(n+2)), "m" (*(n+4)),	\
	 "m" (*(n+5)), "m" (*(n+6)), "m" (*(n+7)) \
	)

#define set_tss_desc(n,addr) _set_tssldt_desc(((char *) (n)),addr,"0x89")
#define set_ldt_desc(n,addr) _set_tssldt_desc(((char *) (n)),addr,"0x82")

#endif
