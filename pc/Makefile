all: pc

CFLAGS := -std=gnu17 -O0 -g -Wall -Werror

pc: main.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: test

test: pc test.d
	./pc test.d
