target := swr

all: $(target)

CFLAGS := -std=gnu11 -g -O0 -ffast-math -Wall -Werror -fsanitize=address -DDEBUG

$(target): main.c
	time $(CC) $(CFLAGS) -o $@ $<

.PHONY: test

test: $(target)
	./$(target)
