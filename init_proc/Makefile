#/*********************************************
# * File name: Makefile
# * Author: Cassidy
# * Time-stamp: <2011-05-15 22:44:47>
# *********************************************
# */

AR	=ar
AS	=as
LD	=ld
LDFLAGS	= -s -x
CC	=gcc -mcpu=i386
CFLAGS	=-Wall -O -fstrength-reduce -fomit-frame-pointer \
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

OBJS = init.o

init_proc.o: $(OBJS)
	$(LD) -r -o init_proc.o $(OBJS)
	sync

clean:
	rm -f *.o
	for i in *.c;do rm -f `basename $$i .c`.s;done

### Dependencies:
init.s init.o: init.c ../include/unistd.h
