TARGET := pdc

all: $(TARGET)

HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))
CFLAGS := -std=gnu11 -O0 -g -ffast-math -Wall -Werror -fsanitize=address

%.o: %.c $(HEADERS)
	time $(CC) -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJECTS)
	time $(CC) $(CFLAGS) -o $@ $^

.PHONY: test

test: $(TARGET)
	./$(TARGET) test.pd
