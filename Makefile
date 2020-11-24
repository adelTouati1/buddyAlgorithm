
CC=gcc
CFLAGS=-g --verbose -std=gnu89 -Wall -Wextra -Wpointer-arith -Wstrict-prototypes -MMD
LIBFLAGS=-I. -shared -fPIC
LIBS=-L. -lbuddy -lm
LIBOBJS=buddy.o
PRELIBOBJS=buddy-pre.o

# This is the full build for both part 1 and part 2 of the buddy memory manager project
# all: libbuddy.so libbuddy-pre.so libbuddy.a buddy-test malloc-test buddy-unit-test

all: libbuddy.so libbuddy.a buddy-test buddy-unit-test

buddy.o: buddy.c
	$(CC) $(CFLAGS) -shared -fPIC -c -o $@ $?

buddy-pre.o: buddy.c
	$(CC) $(CFLAGS) -D BUDDY_PRELOAD -shared -fPIC -c -o $@ $?

libbuddy.so: $(LIBOBJS)
	$(LD) $(LIBFLAGS) -o $@ $?

libbuddy-pre.so: $(PRELIBOBJS)
	$(LD) $(LIBFLAGS) -o $@ $?

libbuddy.a: $(LIBOBJS)
	$(AR)  rcv $@ $(LIBOBJS)
	ranlib $@

buddy-unit-test: buddy-unit-test.o
	$(CC) $(CFLAGS) -o $@ $? $(LIBS)

buddy-test: buddy-test.o
	$(CC) $(CFLAGS) -o $@ $? $(LIBS)

malloc-test: malloc-test.o 
	$(CC) $(CFLAGS) -o $@ $?

clean:	
	/bin/rm -Rf *.o *.d a.out buddy-test malloc-test libbuddy.* libbuddy-pre.* buddy-unit-test
