cmake_minimum_required(VERSION 3.10)
project(huffman_tool LANGUAGES CXX VERSION 1.0.0)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/huffman.cpp
    src/main.cpp
)

# Create executable
add_executable(huffman_tool ${SOURCES})

# Include directories
target_include_directories(huffman_tool PRIVATE src)

# Compiler flags
if(MSVC)
    target_compile_options(huffman_tool PRIVATE /W4 /EHsc)
else()
    target_compile_options(huffman_tool PRIVATE -Wall -Wextra -O2)
endif()

# Install
install(TARGETS huffman_tool RUNTIME DESTINATION bin)
