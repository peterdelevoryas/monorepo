all: swr

swr: main.c
	$(CC) -std=gnu99 -g -O0 -ffast-math -fsanitize=address $< -o $@

.PHONY: test

test: swr
	./swr
