target := ml

all: $(target)

CXXFLAGS := -std=c++20 -g -O2 -ffast-math -fno-exceptions -fsanitize=address -DDEBUG -pedantic -Wall

$(target): main.cpp
	time $(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: test

test: $(target)
	./$(target)
