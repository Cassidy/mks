#/*********************************************
# * File name: Makefile
# * Author: Cassidy
# * Time-stamp: <2011-05-22 22:48:02>
# *********************************************
# */

# Makefile for MKS kernel.
NASM = nasm
AS	=as
LD	=ld
CC	=gcc -march=i386

NASMFLAGS = -w+orphan-labels
LDFLAGS =-m elf_i386 -T kernel.lds -s -x
CFLAGS	=-Wall -fomit-frame-pointer
CPP	=cpp -nostdinc -Iinclude

ARCHIVES=kernel/kernel.o init_proc/init_proc.o system_task/system_task.o
LIBS	=lib/lib.a

.PHONY: all Image system clean burn

.c.s:
	$(CC) $(CFLAGS) \
	-nostdinc -Iinclude -S -o $*.s $<
.s.o:
	$(AS)  -o $*.o $<
.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -Iinclude -c -o $*.o $<

all:    Image

Image:  boot/bootsect boot/setup boot/setup32 system
	dd bs=512 if=boot/bootsect of=Image count=1
	dd bs=512 if=boot/setup of=Image seek=1 count=10
	dd bs=512 if=boot/setup32 of=Image seek=11 count=88
#	objcopy -O binary system core
#	dd bs=512 if=core of=Image seek=5
	dd bs=512 if=system of=Image seek=99

floppy: Image
	dd bs=8192 if=Image of=/dev/fd0
	sync;sync;sync

burn: Image
	dd if=Image of=/tmp/bochs/mks.img conv=notrunc

boot/kernel_start.o: boot/kernel_start.asm
	$(NASM) $(NASMFLAGS) -f elf $< -o $@

system:	boot/kernel_start.o $(ARCHIVES) $(LIBS)
	$(LD) $(LDFLAGS) -M boot/kernel_start.o \
	$(ARCHIVES) $(LIBS) \
	-o system > System.map

kernel/kernel.o:
	(cd kernel; make)
system_task/system_task.o:
	(cd system_task; make)
init_proc/init_proc.o:
	(cd init_proc; make)
lib/lib.a:
	(cd lib; make)

boot/setup32: boot/setup32.asm
	$(NASM) $(NASMFLAGS) -f bin $< -o $@

boot/setup: boot/setup.asm
	$(NASM) $(NASMFLAGS) -f bin $< -o $@

boot/bootsect: boot/bootsect.asm
	$(NASM) $(NASMFLAGS) -f bin $< -o $@

clean:
	rm -f Image System.map boot/bootsect boot/setup boot/setup32
	rm -f system core boot/*.o
	(cd kernel; make clean)
	(cd system_task; make clean)
	(cd init_proc; make clean)
	(cd lib; make clean)

### Dependencies:
