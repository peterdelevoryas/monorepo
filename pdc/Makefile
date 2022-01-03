all: pdc

CXXFLAGS := -std=c++20 -O0 -g -Wall -Werror -fno-exceptions

pdc: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: test

test: pdc test.d
	./pdc test.d
