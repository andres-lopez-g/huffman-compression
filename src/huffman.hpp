#pragma once
#include <string>
#include <vector>
#include <cstdint>

class HuffmanCompressor {
public:
    struct Node {
        uint64_t freq;
        int byte; // -1 interno, 0-255 hoja
        Node* left;
        Node* right;
        Node(uint64_t f, int b, Node* l=nullptr, Node* r=nullptr):freq(f),byte(b),left(l),right(r){}
    };
    // Comprime inputPath y genera outputPath (si vacío, agrega .HUB)
    static bool compress(const std::string& inputPath, const std::string& outputPath = "");
    // Descomprime inputPath (.HUB) y genera outputPath (si vacío, agrega .txt)
    static bool decompress(const std::string& inputPath, const std::string& outputPath = "");
private:
    static void buildCodes(Node* n, const std::string& path, std::vector<std::string>& codes);
    static void freeTree(Node* n);
};
