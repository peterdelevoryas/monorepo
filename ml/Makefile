target := ml

all: $(target)

CFLAGS := -std=c11 -g -O0 -ffast-math -Wall -Werror -fsanitize=address -DDEBUG -pedantic

$(target): main.c
	time $(CC) $(CFLAGS) -o $@ $<

.PHONY: test

test: $(target)
	./$(target)
