CC = gcc
CFLAGS = -Wall -Wextra -O3

hsilop: hsilop.c
	$(CC) $(CFLAGS) $^ -o $@

.EXTRA_PREREQS:= $(abspath $(lastword $(MAKEFILE_LIST)))
