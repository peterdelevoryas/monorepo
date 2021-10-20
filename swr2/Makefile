all: swr

CXXFLAGS := -std=gnu++20 -O0 -fno-exceptions -ffast-math -Wall -Werror
HEADERS := math.hh tga.hh vec.hh vec.cc obj.hh
SOURCES := main.cc math.cc tga.cc obj.cc

swr: $(HEADERS) $(SOURCES)
	time $(CXX) $(CXXFLAGS) $(SOURCES) -o $@

.PHONY: test

test: swr
	./swr
