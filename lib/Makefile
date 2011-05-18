#/*********************************************
# * File name: Makefile
# * Author: Cassidy
# * Time-stamp: <2011-05-15 22:45:22>
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

OBJS = message.o fork.o errno.o

lib.a: $(OBJS)
	$(AR) rcs lib.a $(OBJS)
	sync

clean:
	rm -f *.o *.a
	for i in *.c;do rm -f `basename $$i .c`.s;done

### Dependencies:
message.s message.o: message.c ../include/unistd.h
fork.s fork.o: fork.c
errno.s errno.o: errno.c
