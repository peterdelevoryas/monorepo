TARGET := swr

all: $(TARGET)

HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))
CFLAGS := -std=gnu11 -march=native -O3 -ffast-math -Wall -Werror -fsanitize=address

%.o: %.c $(HEADERS)
	time $(CC) -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJECTS)
	time $(CC) $(CFLAGS) -o $@ $^

.PHONY: test

test: $(TARGET)
	./$(TARGET)
