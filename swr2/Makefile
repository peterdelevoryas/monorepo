all: swr

CXXFLAGS := -std=gnu++20 -O0 -fno-exceptions -ffast-math -Wall -Werror -fsanitize=address
HEADERS := math.hh tga.hh vec.hh obj.hh
SOURCES := main.cc math.cc tga.cc obj.cc

swr: $(HEADERS) $(SOURCES)
	time $(CXX) $(CXXFLAGS) $(SOURCES) -o $@

.PHONY: test llvm

test: swr
	./swr

llvm: $(HEADERS) $(SOURCES)
	$(CXX) -std=gnu++20 -fno-exceptions -ffast-math -O3 -emit-llvm -S $(SOURCES)
