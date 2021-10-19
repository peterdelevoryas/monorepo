all: swr

CFLAGS  := -std=c11 -O0 -fsanitize=address -ffast-math
HEADERS :=
SOURCES := main.c

swr: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $@

.PHONY: test

test: swr
	./swr
