# Author: Aidan Bush
# Assign: Assign 3
# Course: CMPT 360
# Date: Nov. 29, 18
# File: Makefile
# Description: it's a Makefile
SHELL=/bin/bash
CC=gcc
CFLAGS= -Wall -std=c99 -Wextra -g -pedantic -D_POSIX_C_SOURCE=2
LDFLAGS= -lpthread

.PHONY: all test

all: num_server num_client

num_server: num_server.o msg.o buf.o

num_server.o: num_server.c msg.h buf.h

num_client: num_client.o msg.o

num_client.o: num_client.c msg.h

msg.o: msg.c msg.h

buf.o: buf.c buf.h

clean:
	$(RM) num_server num_client *.o
