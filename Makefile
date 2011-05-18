#/*********************************************
# * File name: Makefile
# * Author: Cassidy
# * Time-stamp: <2011-05-15 22:44:03>
# *********************************************
# */

# Makefile for MKS kernel.
AS	=as
LD	=ld
LDFLAGS =-m elf_i386 -Ttext 0 -s -x
CC	=gcc -mcpu=i386
CFLAGS	=-Wall -O2 -fomit-frame-pointer
CPP	=cpp -nostdinc -Iinclude

ARCHIVES=kernel/kernel.o init_proc/init_proc.o system_task/system_task.o
LIBS	=lib/lib.a

.c.s:
	$(CC) $(CFLAGS) \
	-nostdinc -Iinclude -S -o $*.s $<
.s.o:
	$(AS)  -o $*.o $<
.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -Iinclude -c -o $*.o $<

all:    Image

Image:  boot/bootsect boot/setup system
	dd bs=512 if=boot/bootsect of=Image count=1
	dd bs=512 if=boot/setup of=Image seek=1 count=4
	objcopy -O binary system core
	dd bs=512 if=core of=Image seek=5

disk: Image
	dd bs=8192 if=Image of=/dev/fd0
	sync;sync;sync

boot/head.o: boot/head.s
	gcc  -traditional -c boot/head.s
	mv head.o boot/

system:	boot/head.o $(ARCHIVES) $(LIBS)
	$(LD) $(LDFLAGS) -e startup_32 -M boot/head.o \
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

boot/setup: boot/setup.s
	$(AS) -o boot/setup.o boot/setup.s
	$(LD) $(LDFLAGS) -e start --oformat=binary boot/setup.o  -o boot/setup

boot/bootsect: boot/bootsect.s
	$(AS) -o boot/bootsect.o boot/bootsect.s
	$(LD) $(LDFLAGS) -e _start --oformat=binary boot/bootsect.o  -o boot/bootsect

clean:
	rm -f Image System.map bootsect boot/bootsect boot/setup
	rm -f system core boot/*.o
	(cd kernel; make clean)
	(cd system_task; make clean)
	(cd init_proc; make clean)
	(cd lib; make clean)

### Dependencies:
