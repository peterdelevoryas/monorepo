TARGET := n64

all: $(TARGET)

HEADERS  := $(wildcard *.hh)
SOURCES  := $(wildcard *.cc)
OBJECTS  := $(patsubst %.cc,%.o,$(SOURCES))
#OBJECTS += metal.o
FLAGS    := -std=c++20 -g -O0 -fno-exceptions -ffast-math -Wall -Werror
ASAN     := -fsanitize=address
INCLUDES := -Iglfw/include
LIBS     := -lvulkan -lglfw3 -Lglfw/build/src
LIBS     += -framework Cocoa -framework IOKit

metal.o: metal.mm
	$(CXX) $(FLAGS) -c -o $@ $<

%.o: %.cc $(HEADERS)
	$(CXX) $(FLAGS) $(ASAN) $(INCLUDES) -c -o $@ $<

%.ll: %.cc $(HEADERS)
	$(CXX) $(FLAGS) -S -emit-llvm -o $@ $<

%.s: %.cc $(HEADERS)
	$(CXX) $(FLAGS) -S -o $@ $<

$(TARGET): $(OBJECTS)
	$(CXX) $(FLAGS) $(ASAN) $(LIBS) -o $@ $^

.PHONY: test clean

test: $(TARGET)
	$(realpath $(TARGET))

clean:
	rm -f *.o $(TARGET)
