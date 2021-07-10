all: sr

sr: main.cpp
	g++ -std=c++20 main.cpp -Wall -Werror -g -O2 -o $@ -fsanitize=address

.PHONY: test

test: sr
	./sr
	feh out.tga --zoom max -g 1000x1000
