TARGET := ml

all: $(TARGET)

CFLAGS := -std=gnu11 -g -O0 -ffast-math -fsanitize=address -DDEBUG -Wall

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: test

test: $(TARGET)
	./$(TARGET)
