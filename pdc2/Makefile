TARGET := pdc

all: $(TARGET)

HEADERS	:= $(wildcard *.h)
SOURCES	:= $(wildcard *.c)
OBJECTS	:= $(patsubst %.c,%.o,$(SOURCES))
CFLAGS	:= -std=gnu17 -O2 -ffast-math -Wall -Werror
ASAN	:= -fsanitize=address

%.o: %.c $(HEADERS)
	time $(CC) -c $(CFLAGS) $(ASAN) -o $@ $<

%.ll: %.c $(HEADERS)
	time $(CC) $(CFLAGS) -S -emit-llvm -o $@ $<

%.s: %.c $(HEADERS)
	time $(CC) $(CFLAGS) -S -o $@ $<

$(TARGET): $(OBJECTS)
	time $(CC) $(CFLAGS) $(ASAN) -o $@ $^

.PHONY: test clean

test: $(TARGET)
	$(realpath $(TARGET)) test.pd

clean:
	rm -f *.o $(TARGET)
