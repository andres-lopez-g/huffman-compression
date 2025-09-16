#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <array>
#include <memory>
#include <cstdint>

class HuffmanCompressor {
public:
    struct Node {
        uint64_t freq;
        int byte; // -1 for internal nodes, 0-255 for leaves
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        
        Node(uint64_t f, int b) : freq(f), byte(b) {}
        Node(uint64_t f, std::unique_ptr<Node> l, std::unique_ptr<Node> r) 
            : freq(f), byte(-1), left(std::move(l)), right(std::move(r)) {}
    };

private:
    struct NodeComparator {
        bool operator()(const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) const {
            return a->freq > b->freq; // Min-heap
        }
    };

public:
    // Main functions
    static bool compress(const std::string& inputPath, const std::string& outputPath = "");
    static bool decompress(const std::string& inputPath, const std::string& outputPath = "");

private:
    // Helper functions
    static void buildCodes(const Node* node, const std::string& code, std::vector<std::string>& codes);
    static void writeLE(std::ostream& out, uint64_t value, size_t bytes);
    static uint64_t readLE(std::istream& in, size_t bytes);
};
