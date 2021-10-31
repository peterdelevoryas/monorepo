TARGET := pdc

all: $(TARGET)

HEADERS	:= $(wildcard *.h)
SOURCES	:= $(wildcard *.c)
OBJECTS	:= $(patsubst %.c,%.o,$(SOURCES))
CFLAGS	:= -std=gnu17 -O0 -g -ffast-math -Wall -Werror -fsanitize=address

%.o: %.c $(HEADERS)
	time $(CC) -c $(CFLAGS) -o $@ $<

%.ll: %.c $(HEADERS)
	time $(CC) $(CFLAGS) -S -emit-llvm -o $@ $<

%.s: %.c $(HEADERS)
	time $(CC) $(CFLAGS) -S -o $@ $<

$(TARGET): $(OBJECTS)
	time $(CC) $(CFLAGS) -o $@ $^

.PHONY: test clean

test: $(TARGET)
	$(realpath $(TARGET)) test.pd

clean:
	rm -f *.o $(TARGET)
