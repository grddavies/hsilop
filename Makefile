CC = gcc
CFLAGS = -Wall -Wextra -O3 -MMD -MP
OBJS = $(patsubst lib/%.c,build/%.o,$(wildcard lib/*.c))
DEPS = $(OBJS:.o=.d) build/main.d

build/main: build/main.o $(OBJS)
	$(CC) $(CFLAGS) $^ -lreadline -o $@

build/main.o: main.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: lib/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p $@

clean:
	rm -rf build

.PHONY: clean

# Include dependency files
-include $(DEPS)

# Include this Makefile as a dependency of all outputs
.EXTRA_PREREQS:= $(abspath $(lastword $(MAKEFILE_LIST)))
