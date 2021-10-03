all: a.out

a.out: main.c
	$(CC) -std=gnu11 -g $^ -Wall -Werror -O0 -o $@ -fsanitize=address -ffast-math

.PHONY: test test_mac

test: a.out
	./a.out && feh out.tga

test_mac: a.out
	./a.out && open out.tga
