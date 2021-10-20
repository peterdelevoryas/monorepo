all: swr

CXXFLAGS := -std=c++20 -O0 -fno-exceptions -fsanitize=address -ffast-math -pedantic -Wall
HEADERS := math.hh tga.hh
SOURCES := main.cc math.cc tga.cc

swr: $(HEADERS) $(SOURCES)
	time $(CXX) $(CXXFLAGS) $(SOURCES) -o $@

.PHONY: test

test: swr
	./swr
