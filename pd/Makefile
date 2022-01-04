all: pd

CXXFLAGS := -std=c++20 -O0 -g -Wall -Werror -fno-exceptions

pd: main.cc
	@$(CC) $(CXXFLAGS) $< -o $@

.PHONY: test

test: pd test.d
	@./pd test.d
