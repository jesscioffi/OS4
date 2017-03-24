CC=/usr/bin/g++
CFLAGS=-std=c++0x -Wall -ggdb

all: site-tester

site-tester: site-tester.o
	$(CC) $(CFLAGS) -lcurl -lpthread -o site-tester site-tester.o

site-tester.o:
	$(CC) $(CFLAGS) -c site-tester.cpp

clean:
	-rm *.o site-tester
