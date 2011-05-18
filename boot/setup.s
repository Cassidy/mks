/*!
!	setup.s		(C) 1991 Linus Torvalds
!
! setup.s is responsible for getting the system data from the BIOS,
! and putting them into the appropriate places in system memory.
! both setup.s and system has been loaded by the bootblock.
!
! This code asks the bios for memory/disk/other parameters, and
! puts them in a "safe" place: 0x90000-0x901FF, ie where the
! boot-block used to be. It is then up to the protected mode
! system to read them from there before the area is overwritten
! for buffer-blocks.
!

! NOTE! These had better be the same as in bootsect.s!
*/
INITSEG  = 0x9000	#! we move boot here - out of the way
SYSSEG   = 0x1000	#! system loaded at 0x10000 (65536).
SETUPSEG = 0x9020	#! this is the current segment

.code16	
.globl begtext, begdata, begbss, endtext, enddata, endbss, start
.text
begtext:
.data
begdata:
.bss
begbss:
.text

start:
/*
! ok, the read went well so we get current cursor position and save it for
! posterity.

	mov	ax,#INITSEG	! this is done in bootsect already, but...
	mov	ds,ax
	mov	ah,#0x03	! read cursor pos
	xor	bh,bh
	int	0x10		! save it in known place, con_init fetches
	mov	[0],dx		! it from 0x90000.
! Get memory size (extended mem, kB)

	mov	ah,#0x88
	int	0x15
	mov	[2],ax

! Get video-card data:

	mov	ah,#0x0f
	int	0x10
	mov	[4],bx		! bh = display page
	mov	[6],ax		! al = video mode, ah = window width

! check for EGA/VGA and some config parameters

	mov	ah,#0x12
	mov	bl,#0x10
	int	0x10
	mov	[8],ax
	mov	[10],bx
	mov	[12],cx

! Get hd0 data

	mov	ax,#0x0000
	mov	ds,ax
	lds	si,[4*0x41]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0080
	mov	cx,#0x10
	rep
	movsb

! Get hd1 data

	mov	ax,#0x0000
	mov	ds,ax
	lds	si,[4*0x46]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0090
	mov	cx,#0x10
	rep
	movsb

! Check that there IS a hd1 :-)

	mov	ax,#0x01500
	mov	dl,#0x81
	int	0x13
	jc	no_disk1
	cmp	ah,#3
	je	is_disk1
*/
	movw	$INITSEG, %ax
	movw	%ax, %ds
	movb	$0x03, %ah
	xorb	%bh, %bh
	int	$0x10
	movw	%dx, (0)

	movb	$0x88, %ah
	int	$0x15
	movw	%ax, (2)

	movb	$0x0f, %ah
	int	$0x10
	movw	%bx, (4)
	movw	%ax, (6)

	movb	$0x12, %ah
	movb	$0x10, %bl
	int	$0x10
	movw	%ax, (8)
	movw	%bx, (10)
	movw	%cx, (12)

	movw	$0x0000, %ax
	movw	%ax, %ds
	ldsw	(4*0x41), %si
	movw	$INITSEG, %ax
	movw	%ax, %es
	movw	$0x0080, %di
	movw	$0x10, %cx
	rep
	movsb

	movw	$0x0000, %ax
	movw	%ax, %ds
	ldsw	(4*0x46), %si
	movw	$INITSEG, %ax
	movw	%ax, %es
	movw	$0x0090, %di
	movw	$0x10, %cx
	rep
	movsb

	movw	$0x01500, %ax
	movb	$0x81, %dl
	int	$0x13
	jc	no_disk1
	cmpb	$3, %ah
	je	is_disk1	
no_disk1:
	movw	$INITSEG, %ax
	movw	%ax, %es
	movw	$0x0090, %di
	movw	$0x10, %cx
	movw	$0x00, %ax
	rep
	stosb
is_disk1:
	cli

	movw	$0x0000, %ax
	cld
do_move:
	movw	%ax, %es
	addw	$0x1000, %ax
	cmpw	$0x9000, %ax
	jz	end_move
	movw	%ax, %ds
	subw	%di, %di
	subw	%si, %si
	movw 	$0x8000, %cx
	rep
	movsw
	jmp	do_move

end_move:
	movw	$SETUPSEG, %ax
	movw	%ax, %ds
	lidt	idt_48
	lgdt	gdt_48

	call	empty_8042
	movb	$0xD1, %al
	outb	%al, $0x64
	call	empty_8042
	movb	$0xDF, %al
	outb	%al, $0x60
	call	empty_8042

	movb	$0x11, %al
	outb	%al, $0x20
	.word	0x00eb,0x00eb
	outb	%al, $0xA0
	.word	0x00eb,0x00eb
	movb	$0x20, %al
	outb	%al, $0x21
	.word	0x00eb,0x00eb
	movb	$0x28, %al
	outb	%al, $0xA1
	.word	0x00eb,0x00eb
	movb	$0x04, %al
	outb	%al, $0x21
	.word	0x00eb,0x00eb
	movb	$0x02, %al
	outb	%al, $0xA1
	.word	0x00eb,0x00eb
	movb	$0x01, %al
	outb	%al, $0x21
	.word	0x00eb,0x00eb
	outb	%al, $0xA1
	.word	0x00eb,0x00eb
	movb	$0xFF, %al
	outb	%al, $0x21
	.word	0x00eb,0x00eb
	outb	%al, $0xA1

	movw	$0x0001, %ax
	lmsw	%ax
	ljmp	$8, $0
/*
! This routine checks that the keyboard command queue is empty
! No timeout is used - if this hangs there is something wrong with
! the machine, and we probably couldn't proceed anyway.
empty_8042:
	.word	0x00eb,0x00eb
	in	al,#0x64	! 8042 status port
	test	al,#2		! is input buffer full?
	jnz	empty_8042	! yes - loop
	ret
*/

empty_8042:
	.word	0x00eb,0x00eb
	inb	$0x64, %al
	testb	$2, %al
	jnz	empty_8042
	ret

gdt:
	.word	0,0,0,0		#! dummy

	.word	0x07FF		#! 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x0000		#! base address=0
	.word	0x9A00		#! code read/exec
	.word	0x00C0		#! granularity=4096, 386

	.word	0x07FF		#! 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x0000		#! base address=0
	.word	0x9200		#! data read/write
	.word	0x00C0		#! granularity=4096, 386

idt_48:
	.word	0			#! idt limit=0
	.word	0,0			#! idt base=0L

gdt_48:
	.word	0x800		#! gdt limit=2048, 256 GDT entries
	.word	512+gdt,0x9	#! gdt base = 0X9xxxx
	
.text
endtext:
.data
enddata:
.bss
endbss:
