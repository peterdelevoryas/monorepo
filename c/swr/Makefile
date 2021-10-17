all: swr

swr: main.c
	$(CC) -std=gnu99 -O2 -ffast-math -fsanitize=address $< -o $@

.PHONY: test

test: swr
	./swr
