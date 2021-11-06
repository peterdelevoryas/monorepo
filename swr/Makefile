TARGET := swr

all: $(TARGET)

CXXFLAGS := -std=c++20 -O2 -fno-exceptions -ffast-math -Wall -Werror -fsanitize=address

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: test clean

test: $(TARGET)
	$(realpath $(TARGET))

clean:
	rm -f *.o $(TARGET)
