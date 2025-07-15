CC = gcc
CFLAGS = -Wall -Wextra -O3
OBJS = $(patsubst lib/%.c,build/%.o,$(wildcard lib/*.c))

build/hsilop: main.c $(OBJS)
	$(CC) $(CFLAGS) $^ -lreadline -o $@

build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $^ -o $@

build:
	mkdir -p $@

# Include this Makefile as a dependency of all outputs
.EXTRA_PREREQS:= $(abspath $(lastword $(MAKEFILE_LIST)))
