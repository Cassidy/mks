/*********************************************
 * File name: asm.s
 * Author: Cassidy
 * Time-stamp: <2011-06-10 12:46:00>
 *********************************************
 */

.text
.globl intr0, intr1, intr2, intr3, intr4, intr5, intr6, intr7
.globl intr8, intr9, intr10, intr11, intr12, intr13, intr14, intr15
.globl intr16, intr17, intr18, intr19
.globl hwintr0, hwintr1, hwintr2, hwintr3, hwintr4, hwintr5
.globl hwintr6, hwintr7, hwintr8, hwintr9, hwintr10, hwintr11
.globl hwintr12, hwintr13, hwintr14, hwintr15
.globl intr_msg, intr_kercall, intr_reserved


intr0:
	pushl $0
	jmp no_error_code
intr1:
	pushl $1
	jmp no_error_code
intr2:
	pushl $2
	jmp no_error_code
intr3:
	pushl $3
	jmp no_error_code
intr4:
	pushl $4
	jmp no_error_code
intr5:
	pushl $5
	jmp no_error_code
intr6:
	pushl $6
	jmp no_error_code
intr7:
	pushl $7
	jmp no_error_code
intr8:
	pushl $8
	jmp with_error_code
intr9:
	pushl $9
	jmp no_error_code
intr10:
	pushl $10
	jmp with_error_code
intr11:
	pushl $11
	jmp with_error_code
intr12:
	pushl $12
	jmp with_error_code
intr13:
	pushl $13
	jmp with_error_code
intr14:
	pushl $14
	jmp with_error_code
intr15:
	pushl $15
	jmp no_error_code
intr16:
	pushl $16
	jmp no_error_code
intr17:
	pushl $17
	jmp no_error_code
intr18:
	pushl $18
	jmp no_error_code
intr19:
	pushl $19
	jmp no_error_code

intr32:
hwintr0:
	pushl $32
	jmp no_error_code
intr33:
hwintr1:
	pushl $33
	jmp no_error_code
intr34:
hwintr2:
	pushl $34
	jmp no_error_code
intr35:
hwintr3:
	pushl $35
	jmp no_error_code
intr36:
hwintr4:
	pushl $36
	jmp no_error_code
intr37:
hwintr5:
	pushl $37
	jmp no_error_code
intr38:
hwintr6:
	pushl $38
	jmp no_error_code
intr39:
hwintr7:
	pushl $39
	jmp no_error_code
intr40:
hwintr8:
	pushl $40
	jmp no_error_code
intr41:
hwintr9:
	pushl $41
	jmp no_error_code
intr42:
hwintr10:
	pushl $42
	jmp no_error_code
intr43:
hwintr11:
	pushl $43
	jmp no_error_code
intr44:
hwintr12:
	pushl $44
	jmp no_error_code
intr45:
hwintr13:
	pushl $45
	jmp no_error_code
intr46:
hwintr14:
	pushl $46
	jmp no_error_code
intr47:
hwintr15:
	pushl $47
	jmp no_error_code

intr_msg:
	pushl %edx
	pushl %edi
	pushl %esi
	pushl %ebp
	push %ds
	push %es
	push %fs
	movl $0x10, %edx
	mov %dx, %ds
	mov %dx, %es
	mov %dx, %fs
	pushl %ecx
	movl %esp, %ecx
	pushl %ebx
	movl %esp, %ebx
	pushl %ecx
	pushl %ebx
	pushl %eax
	call do_intr_msg
	addl $12, %esp
	popl %ebx
	popl %ecx
	pop %fs
	pop %es
	pop %ds
	popl %ebp
	popl %esi
	popl %edi
	popl %edx
	iret


intr_kercall:
	pushl $0x88
	jmp no_error_code

intr_reserved:
	pushl $255
	jmp no_error_code

no_error_code:
	xchgl %eax, (%esp) /* 交换eax和esp所指位置的值（eax=向量号 esp=原eax） */
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi
	pushl %ebp
	push %ds
	push %es
	push %fs

	movl 44(%esp), %edx /* edx = cs */
	andl $3, %edx /* edx: 中断前 cs 的请求特权级 */
	pushl %edx

	pushl $0 /* 错误码为 0 */
	lea 48(%esp), %edx /* edx = &eip（被中断程序 eip 值的地址，即堆栈指针） */
	pushl %edx
	movl $0x10, %edx
	mov %dx, %ds
	mov %dx, %es
	mov %dx, %fs
	call *intr_table(, %eax, 4)
	addl $12, %esp
	pop %fs
	pop %es
	pop %ds
	popl %ebp
	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret

with_error_code:
	xchgl %eax, 4(%esp)
	xchgl %ebx, (%esp)
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi
	pushl %ebp
	push %ds
	push %es
	push %fs

	movl 44(%esp), %edx
	andl $3, %edx
	pushl %edx

	pushl %eax
	lea 48(%esp), %eax
	pushl %eax
	movl $0x10, %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	call *intr_table(, %ebx, 4)
	addl $12, %esp
	pop %fs
	pop %es
	pop %ds
	popl %ebp
	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret
