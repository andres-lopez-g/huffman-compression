CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic
SRC_DIR := src
OBJS := $(SRC_DIR)/main.o $(SRC_DIR)/huffman.o
BIN := huffman_tool

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SRC_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/huffman.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SRC_DIR)/huffman.o: $(SRC_DIR)/huffman.cpp $(SRC_DIR)/huffman.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean
