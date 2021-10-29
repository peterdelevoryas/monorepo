TARGET := pdc

all: $(TARGET)

CFLAGS := -std=gnu11 -O0 -g -ffast-math -Wall -Werror -fsanitize=address

$(TARGET): main.c
	time $(CC) $(CFLAGS) $< -o $@

.PHONY: test

test: $(TARGET)
	./$(TARGET) test.pd
