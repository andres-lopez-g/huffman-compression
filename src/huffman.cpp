#include "huffman.hpp"
#include <fstream>
#include <queue>
#include <array>
#include <iostream>
#include <filesystem>
#include <climits>

namespace {
    struct NodeCmp {
        bool operator()(HuffmanCompressor::Node* a, HuffmanCompressor::Node* b) const {
            return a->freq > b->freq; // menor frecuencia tiene mayor prioridad
        }
    };
    void writeLE(std::ostream& os, uint64_t v, size_t bytes) {
        for(size_t i=0;i<bytes;++i) os.put(static_cast<char>((v>>(8*i)) & 0xFF));
    }
    uint64_t readLE(std::istream& is, size_t bytes) {
        uint64_t v=0;
        for(size_t i=0;i<bytes;++i) {
            int c = is.get();
            if(c==EOF) return UINT64_MAX;
            v |= (uint64_t(uint8_t(c)) << (8*i));
        }
        return v;
    }
}

void HuffmanCompressor::buildCodes(Node* n, const std::string& path, std::vector<std::string>& codes) {
    if(!n) return;
    if(n->byte >= 0) {
        codes[static_cast<uint8_t>(n->byte)] = path.empty() ? "0" : path;
        return;
    }
    buildCodes(n->left, path + '0', codes);
    buildCodes(n->right, path + '1', codes);
}

void HuffmanCompressor::freeTree(Node* n) {
    if(!n) return;
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

bool HuffmanCompressor::compress(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream in(inputPath, std::ios::binary);
    if(!in) {
        std::cerr << "No se pudo abrir: " << inputPath << "\n";
        return false;
    }
    std::array<uint64_t,256> freq{};
    std::vector<uint8_t> data;
    char c;
    while(in.get(c)) {
        uint8_t b = static_cast<uint8_t>(c);
        freq[b]++;
        data.push_back(b);
    }
    uint64_t originalSize = data.size();
    if(originalSize == 0) {
        std::cerr << "Archivo vacío.\n";
        return false;
    }
    // Construir árbol
    std::priority_queue<Node*, std::vector<Node*>, NodeCmp> pq;
    int symbolCount=0;
    for(int i=0;i<256;++i) {
        if(freq[i]) {
            pq.push(new Node(freq[i], i));
            symbolCount++;
        }
    }
    while(pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        pq.push(new Node(a->freq + b->freq, -1, a, b));
    }
    Node* root = pq.top();

    std::vector<std::string> codes(256);
    buildCodes(root, "", codes);

    // Escribir salida
    std::string outPath = outputPath.empty() ? (inputPath + ".HUB") : outputPath;
    std::ofstream out(outPath, std::ios::binary);
    if(!out) {
        std::cerr << "No se pudo crear: " << outPath << "\n";
        freeTree(root);
        return false;
    }
    // Magic
    out.write("HUB1",4);
    // Tamaño original
    writeLE(out, originalSize, 8);
    // Número de símbolos
    writeLE(out, symbolCount, 2);
    // Tabla (byte + frecuencia 8 bytes c/u)
    for(int i=0;i<256;++i) {
        if(freq[i]) {
            out.put(static_cast<char>(i));
            writeLE(out, freq[i], 8);
        }
    }
    // Codificar
    uint8_t buffer=0;
    int bits=0;
    uint64_t totalBits=0;
    for(uint8_t b: data) {
        const std::string& code = codes[b];
        for(char bit : code) {
            buffer <<= 1;
            if(bit=='1') buffer |= 1;
            bits++;
            totalBits++;
            if(bits==8) {
                out.put(static_cast<char>(buffer));
                buffer=0;
                bits=0;
            }
        }
    }
    if(bits>0) {
        buffer <<= (8-bits);
        out.put(static_cast<char>(buffer));
    }
    // Escribimos longitud de bits al final para exactitud
    writeLE(out, totalBits, 8);

    out.close();
    freeTree(root);

    std::uintmax_t outSize = std::filesystem::file_size(outPath);
    std::cout << "Original: " << originalSize << " bytes, Comprimido: " << outSize << " bytes\n";
    return true;
}

bool HuffmanCompressor::decompress(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream in(inputPath, std::ios::binary);
    if(!in) {
        std::cerr << "No se pudo abrir: " << inputPath << "\n";
        return false;
    }
    char magic[4];
    in.read(magic,4);
    if(in.gcount()!=4 || std::string(magic,4)!="HUB1") {
        std::cerr << "Formato inválido.\n";
        return false;
    }
    uint64_t originalSize = readLE(in,8);
    uint64_t symbolCount = readLE(in,2);
    if(originalSize==UINT64_MAX || symbolCount==UINT64_MAX) {
        std::cerr << "Cabecera corrupta.\n";
        return false;
    }
    std::array<uint64_t,256> freq{};
    for(uint64_t i=0;i<symbolCount;++i) {
        int b = in.get();
        if(b==EOF) { std::cerr<<"Tabla truncada.\n"; return false; }
        uint64_t f = readLE(in,8);
        if(f==UINT64_MAX) { std::cerr<<"Tabla truncada.\n"; return false; }
        freq[static_cast<uint8_t>(b)] = f;
    }
    // Reconstruir árbol
    std::priority_queue<Node*, std::vector<Node*>, NodeCmp> pq;
    for(int i=0;i<256;++i) if(freq[i]) pq.push(new Node(freq[i], i));
    if(pq.empty()) {
        std::cerr << "Sin símbolos.\n";
        return false;
    }
    while(pq.size()>1) {
        Node* a=pq.top(); pq.pop();
        Node* b=pq.top(); pq.pop();
        pq.push(new Node(a->freq + b->freq, -1, a, b));
    }
    Node* root = pq.top();

    // Leer todo el resto excepto últimos 8 bytes (longitud bits)
    std::streampos posBeforeBitsLen = in.tellg();
    in.seekg(0,std::ios::end);
    std::streampos fileEnd = in.tellg();
    if(fileEnd - posBeforeBitsLen < 8) {
        std::cerr << "Archivo truncado.\n";
        freeTree(root);
        return false;
    }
    // Colocarse 8 bytes antes del final para leer la longitud de bits
    {
        std::streamoff offEnd = static_cast<std::streamoff>(fileEnd);
        in.seekg(offEnd - static_cast<std::streamoff>(8));
    }
    uint64_t totalBits = readLE(in,8);
    in.seekg(posBeforeBitsLen);

    std::vector<uint8_t> encoded;
    size_t dataBytes = 0;
    {
        std::streamoff offEnd = static_cast<std::streamoff>(fileEnd);
        std::streamoff offStart = static_cast<std::streamoff>(posBeforeBitsLen);
        dataBytes = static_cast<size_t>((offEnd - 8) - offStart);
    }
    encoded.resize(dataBytes);
    in.read(reinterpret_cast<char*>(encoded.data()), dataBytes);

    std::string outPath = outputPath.empty() ? (inputPath + ".txt") : outputPath;
    std::ofstream out(outPath, std::ios::binary);
    if(!out) {
        std::cerr << "No se pudo crear: " << outPath << "\n";
        freeTree(root);
        return false;
    }

    Node* cur = root;
    uint64_t produced=0;
    uint64_t bitsRead=0;
    for(uint8_t byte : encoded) {
        for(int i=7;i>=0 && bitsRead < totalBits; --i) {
            int bit = (byte >> i) & 1;
            cur = bit ? cur->right : cur->left;
            bitsRead++;
            if(cur->byte >=0) {
                out.put(static_cast<char>(cur->byte));
                produced++;
                cur = root;
                if(produced == originalSize) break;
            }
        }
        if(produced == originalSize) break;
    }
    out.close();
    freeTree(root);
    if(produced != originalSize) {
        std::cerr << "Advertencia: tamaño reconstruido ("<<produced<<") difiere del esperado ("<<originalSize<<").\n";
        return false;
    }
    std::cout << "Descompresión completa: " << outPath << "\n";
    return true;
}
