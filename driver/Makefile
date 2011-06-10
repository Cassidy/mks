# Makefile for driver
LD	=ld
LDFLAGS	= -s -x

CC	=gcc -march=i386
CFLAGS	=-Wall -fstrength-reduce -fomit-frame-pointer \
	-finline-functions -nostdinc -I../include

OBJS = character/keyboard.o

.PHONY: clean

driver.o: $(OBJS)
	$(LD) -r -o $@ $(OBJS)
	sync

character/keyboard.o: character/keyboard.c character/keyboard.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o character/*.o block/*.o
	for i in *.c;do rm -f `basename $$i .c`.s;done

# End of Makefile
