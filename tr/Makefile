all: a.out

CXXFLAGS := -std=c++20 -O0 -g -Wall -Werror -fno-exceptions -ffast-math

a.out: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: test

test: a.out
	./a.out
