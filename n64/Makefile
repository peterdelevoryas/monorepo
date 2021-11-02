TARGET := n64

all: $(TARGET)

HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
CFLAGS  := -std=c++20 -O2 -fno-exceptions -ffast-math -Wall -Werror
ASAN    := -fsanitize=address
LIBS    := -lvulkan

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(ASAN) -c -o $@ $<

%.ll: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -S -emit-llvm -o $@ $<

%.s: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -S -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(ASAN) $(LIBS) -o $@ $^

.PHONY: test clean

test: $(TARGET)
	$(realpath $(TARGET))

clean:
	rm -f *.o $(TARGET)
