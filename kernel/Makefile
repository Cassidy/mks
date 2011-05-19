#/*********************************************
# * File name: Makefile
# * Author: Cassidy
# * Time-stamp: <2011-05-16 18:51:53>
# *********************************************
# */

AR	=ar
AS	=as
LD	=ld
LDFLAGS	= -s -x
CC	=gcc -mcpu=i386
CFLAGS	=-Wall -fstrength-reduce -fomit-frame-pointer \
	-finline-functions -nostdinc -I../include
CPP	=gcc -E -nostdinc -I../include

.c.s:
	$(CC) $(CFLAGS) \
	-S -o $*.s $<
.s.o:
	$(AS) -o $*.o $<
.c.o:
	$(CC) $(CFLAGS) \
	-c -o $*.o $<

OBJS = main.o printk.o interrupt.o asm.o time.o proc.o memory.o messaging.o

kernel.o: $(OBJS)
	$(LD) -r -o kernel.o $(OBJS)
	sync

clean:
	rm -f *.o
	for i in *.c;do rm -f `basename $$i .c`.s;done

### Dependencies:
main.s main.o: main.c ../include/kernel/kernel.h ../include/asm/system.h
printk.s printk.o: printk.c
interrupt.s interrupt.o: interrupt.c ../include/kernel/kernel.h \
   ../include/kernel/interrupt.h ../include/kernel/head.h \
   ../include/asm/system.h ../include/asm/io.h 
asm.o: asm.s
time.s time.o: time.c ../include/asm/io.h ../include/kernel/time.h \
   ../include/kernel/kernel.h ../include/kernel/proc.h
proc.s proc.o: proc.c ../include/kernel/kernel.h ../include/kernel/proc.h
memory.s memory.o: memory.c ../include/kernel/kernel.h
messaging.s messaging.o: messaging.c ../include/kernel/kernel.h \
   ../include/kernel/messaging.h ../include/kernel/proc.h
