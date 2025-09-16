# Makefile simplificado para C++
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/huffman.cpp
TARGET = huffman_tool.exe

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✅ Compilación completada: $(TARGET)"

clean:
	@if exist $(TARGET) del $(TARGET)
	@echo "🧹 Limpieza completada"

run: $(TARGET)
	@echo "🚀 Ejecutando programa..."
	@$(TARGET)
