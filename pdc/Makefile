all: pdc

pdc: main.cpp
	g++ -std=c++20 main.cpp -o $@ -g -O2 -Wall -Werror

.PHONY: test

test: pdc
	./pdc test.pd
