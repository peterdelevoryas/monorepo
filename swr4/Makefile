all: a.out

CXXFLAGS := -std=c++20 -g -O0 -ffast-math -fno-exceptions -Wall -Werror -fsanitize=address

a.out: main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: test

test: a.out
	./a.out
