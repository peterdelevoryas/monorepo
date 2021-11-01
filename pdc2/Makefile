TARGET := pdc

all: $(TARGET)

HEADERS	:= $(wildcard *.h)
SOURCES	:= $(wildcard *.c)
OBJECTS	:= $(patsubst %.c,%.o,$(SOURCES))
CFLAGS	:= -std=gnu17 -O2 -ffast-math -Wall -Werror
ASAN	:= -fsanitize=address

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(ASAN) -c -o $@ $<

%.ll: %.c $(HEADERS)
	$(CC) $(CFLAGS) -S -emit-llvm -o $@ $<

%.s: %.c $(HEADERS)
	$(CC) $(CFLAGS) -S -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(ASAN) -o $@ $^

.PHONY: test clean

test: $(TARGET)
	$(realpath $(TARGET)) test.pd

clean:
	rm -f *.o $(TARGET)
