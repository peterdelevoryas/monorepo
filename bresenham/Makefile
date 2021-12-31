all: a.out

CFLAGS := -std=gnu17 -O0 -Wall -Werror -g

a.out: main.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: test

test: a.out
	./a.out
