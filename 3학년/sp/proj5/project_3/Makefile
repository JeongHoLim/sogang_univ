CC = gcc
CFLAGS=-O2 -Wall
LDLIBS = -lpthread

all: multiclient1 stockclient thread event

multiclient1: multiclient1.c csapp.c csapp.h
stockclient: stockclient.c csapp.c csapp.h
event: event.c csapp.c csapp.h
thread: thread.c csapp.c csapp.h

clean:
	rm -rf *~ multiclient1 stockclient thread event *.o
