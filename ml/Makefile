TARGET := ml
TARGET2 := ml2

all: $(TARGET) $(TARGET2)

CFLAGS := -std=c11 -g -O2 -ffast-math -fsanitize=address -DDEBUG -pedantic -Wall
CXXFLAGS := -std=c++20 -g -O0 -ffast-math -fsanitize=address -DDEBUG -pedantic -Wall

$(TARGET): main.c
	time $(CC) $(CFLAGS) -o $@ $<

$(TARGET2): main.cpp
	time $(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: test test2

test: $(target)
	./$(target)

test2: $(TARGET2)
	./$(TARGET2)
