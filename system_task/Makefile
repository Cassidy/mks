#/*********************************************
# * File name: Makefile
# * Author: Cassidy
# * Time-stamp: <2011-05-15 22:45:42>
# *********************************************
# */

AR	=ar
AS	=as
LD	=ld
LDFLAGS	= -s -x
CC	=gcc -march=i386
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

OBJS = system.o

system_task.o: $(OBJS)
	$(LD) -r -o system_task.o $(OBJS)
	sync

clean:
	rm -f *.o
	for i in *.c;do rm -f `basename $$i .c`.s;done

### Dependencies:
system.s system.o: system.c ../include/unistd.h
