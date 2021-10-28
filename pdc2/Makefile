TARGET := pdc

all: $(TARGET)

CXXFLAGS := -std=gnu++20 -O0 -fno-exceptions -ffast-math -Wall -Werror -fsanitize=address

$(TARGET): main.cpp
	time $(CXX) $(CXXFLAGS) $< -o $@

.PHONY: test

test: $(TARGET)
	./$(TARGET) test.pd
