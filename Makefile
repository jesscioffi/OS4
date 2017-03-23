CC=/usr/bin/g++
CFLAGS=-Wall

all: site-tester

site-tester: site-tester.o
	$(CC) -o site-tester site-tester.o

site-tester.o:
	$(CC) $(CFLAGS) -c site-tester.cpp

clean:
	-rm *.o site-tester
