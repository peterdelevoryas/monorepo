all: swr

CFLAGS  := -std=c11 -O0 -fsanitize=address -ffast-math -pedantic -Wall
HEADERS := math.h tga.h
SOURCES := main.c math.c tga.c

swr: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $@

.PHONY: test

test: swr
	./swr
