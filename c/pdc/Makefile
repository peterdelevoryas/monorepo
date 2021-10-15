exe := pdc

all: $(exe)

$(exe): main.c
	$(CC) -std=gnu99 -g -O0 -ffast-math -fsanitize=address $< -o $@

.PHONY: test

test: $(exe)
	./$(exe)
