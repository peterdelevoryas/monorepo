all: sr

sr: main.cpp
	g++ -std=c++20 main.cpp -Wall -Werror -g -O2 -o $@

.PHONY: test

test: sr
	./sr
