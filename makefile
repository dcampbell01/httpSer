#Author: Jeff Bunce
#File type: Linux makefile
#Purpose: Simplify the compilation process

#Options
CC=gcc
DEBUG=-g
CFLAGS=-Wall
FILES=server.c
SERVERFLAGS=-o server -lpthread


debug : $(FILES)
	echo;echo;
	$(CC) $(DEBUG) $(FILES) $(SERVERFLAGS)

release : $(FILES)
	echo;echo;
	$(CC) $(CFLAGS) $(FILES) $(SERVERFLAGS)

clean :
	rm server; rm a.out; rm *~; rm core.*; rm *.gch; echo; echo; ls; echo;