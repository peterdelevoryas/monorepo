all: pc

CFLAGS := -std=gnu17 -O0 -g -Wall -Werror
LLVM := $(shell llvm-config --cflags --ldflags --libs core x86codegen --system-libs)
LDFLAGS := -fno-rtti -lc++ $(LLVM)

pc: main.c
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: test

test: pc test.d
	./pc test.d
