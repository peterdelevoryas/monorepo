all: pdc

pdc: main.cpp
	$(CXX) -std=c++20 -g $^ -Werror -O0 -o $@ -fno-exceptions -fsanitize=address -ffast-math

.PHONY: test

test: pdc
	./pdc test.pd
