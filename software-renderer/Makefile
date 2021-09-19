all: a.out

a.out: main.cpp
	$(CXX) -std=c++20 -g $^ -Wall -Werror -O2 -o $@ -fsanitize=address -ffast-math -fno-exceptions

.PHONY: test

test: a.out
	./a.out && feh out.tga
