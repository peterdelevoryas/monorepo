target := ml

all: $(target)

CFLAGS := -std=c11 -g -O0 -ffast-math -fsanitize=address -DDEBUG -pedantic -Wall

$(target): main.c
	time $(CC) $(CFLAGS) -o $@ $<

.PHONY: test

test: $(target)
	./$(target)
