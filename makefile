CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
TARGET = translator
SOURCES = hashtable.cpp main.cpp
HEADERS = $(wildcard *.h)
.PHONY: all test clean
all: $(TARGET)
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@
test: $(TARGET) test_hash
	python3 tests/test_cli.py ./$(TARGET)
	./test_hash
clean:
	rm -f $(TARGET) test_hash *.o

test_hash: tests/test_hash.cpp hashtable.cpp hashtable.h
	$(CXX) $(CXXFLAGS) tests/test_hash.cpp hashtable.cpp -o $@
