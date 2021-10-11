all: a.out

a.out: main.c
	$(CC) -std=gnu99 -g $^ -Wall -Werror -O0 -o $@ -fsanitize=address -ffast-math

.PHONY: test

test: a.out
	./a.out
