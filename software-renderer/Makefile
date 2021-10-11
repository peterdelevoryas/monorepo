all: a.out

a.out: main.c
	$(CC) -std=gnu11 -g $^ -Wall -Werror -O0 -o $@ -fsanitize=address -ffast-math

.PHONY: test

IMAGE_VIEWER := feh
ifeq ($(shell uname -s),Darwin)
	IMAGE_VIEWER := open
endif

test: a.out
	./a.out && $(IMAGE_VIEWER) out.tga
