TARGET := pdc

all: $(TARGET)

HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
FLAGS 	:= -std=c++20 -O0 -g -fno-exceptions -ffast-math -Wall -Werror -fsanitize=address -march=native

%.o: %.cpp $(HEADERS)
	time $(CXX) -c $(FLAGS) -o $@ $<

$(TARGET): $(OBJECTS)
	time $(CXX) $(FLAGS) -o $@ $^

.PHONY: test clean

test: $(TARGET)
	$(realpath $(TARGET)) test.pd

clean:
	rm -f $(OBJECTS) $(TARGET)
