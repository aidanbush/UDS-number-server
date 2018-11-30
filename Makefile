# Author: Aidan Bush
# Assign: Assign 3
# Course: CMPT 360
# Date: Nov. 29, 18
# File: Makefile
# Description: it's a Makefile
SHELL=/bin/bash
CC=gcc
CFLAGS= -Wall -std=c99 -Wextra -g -pedantic -D_POSIX_C_SOURCE=2

.PHONY: all test

all: num_server num_client

num_server: num_server.c

num_client: num_client.c

clean:
	$(RM) num_server *.o
