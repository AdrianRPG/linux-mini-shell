# Makefile for mysh
#
#   make          build ./mysh
#   make leak     build ./mysh_leak, the same source with -DLEAK
#   make clean    remove both binaries and the test files t1.txt, t2.txt, list.txt
#
# The indented lines below begin with a real TAB character.

CC     = gcc
CFLAGS = -Wall -Wextra -O0 -g

all: mysh

mysh: mysh.c
	$(CC) $(CFLAGS) -o mysh mysh.c

leak: mysh.c
	$(CC) $(CFLAGS) -DLEAK -o mysh_leak mysh.c

clean:
	rm -f mysh mysh_leak t1.txt t2.txt list.txt

.PHONY: all leak clean
