TARGET := swr

all: $(TARGET)

HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))

OSX_SDK := /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
CC := /Users/pdel/llvm-project/build/bin/clang
CFLAGS := -std=gnu11 -march=native -O3 -ffast-math -fsanitize=address -Wall -Werror
CFLAGS += -I$(OSX_SDK)/usr/include
CFLAGS += -Wno-nullability-completeness -DTARGET_OS_IPHONE=0

%.o: %.c $(HEADERS)
	time $(CC) -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJECTS)
	time $(CC) $(CFLAGS) -o $@ $^ -L$(OSX_SDK)/usr/lib

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS)
	rm $(TARGET)
