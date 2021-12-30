all: a.out

CFLAGS := -std=gnu17 -g -O0 -ffast-math -fno-exceptions -Wall -Werror

a.out: main.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: test

test: a.out
	./a.out
