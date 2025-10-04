#include "huffman.hpp"
#include <filesystem>
#include <iomanip>

void HuffmanCompressor::buildCodes(const Node* node, const std::string& code, std::vector<std::string>& codes) {
    if (!node) return;
    
    if (node->byte >= 0) {
        codes[static_cast<uint8_t>(node->byte)] = code.empty() ? "0" : code;
        return;
    }
    
    buildCodes(node->left.get(), code + '0', codes);
    buildCodes(node->right.get(), code + '1', codes);
}

void HuffmanCompressor::writeLE(std::ostream& out, uint64_t value, size_t bytes) {
    for (size_t i = 0; i < bytes; ++i) {
        out.put(static_cast<char>((value >> (8 * i)) & 0xFF));
    }
}

uint64_t HuffmanCompressor::readLE(std::istream& in, size_t bytes) {
    uint64_t value = 0;
    for (size_t i = 0; i < bytes; ++i) {
        int byte = in.get();
        if (byte == EOF) return UINT64_MAX;
        value |= (static_cast<uint64_t>(byte) << (8 * i));
    }
    return value;
}

bool HuffmanCompressor::compress(const std::string& inputPath, const std::string& outputPath) {
    std::cout << "\nIniciando compresion...\n";
    
    // Abrir archivo de entrada
    std::ifstream input(inputPath, std::ios::binary);
    if (!input) {
        std::cerr << "Error: No se pudo abrir el archivo: " << inputPath << "\n";
        return false;
    }

    // Leer datos
    std::vector<uint8_t> data;
    char byte;
    while (input.get(byte)) {
        data.push_back(static_cast<uint8_t>(byte));
    }
    input.close();

    if (data.empty()) {
        std::cerr << "Error: El archivo esta vacio.\n";
        return false;
    }

    uint64_t originalSize = data.size();
    std::cout << "Tamanio original: " << originalSize << " bytes\n";

    // Construir tabla de frecuencias
    std::array<uint64_t, 256> freq{};
    for (uint8_t b : data) {
        freq[b]++;
    }

    // Construir árbol de Huffman
    std::priority_queue<std::unique_ptr<Node>, std::vector<std::unique_ptr<Node>>, NodeComparator> pq;
    int symbolCount = 0;

    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            pq.push(std::make_unique<Node>(freq[i], i));
            symbolCount++;
        }
    }

    if (symbolCount == 1) {
        // Caso especial: solo un simbolo unico
        auto root = std::make_unique<Node>(freq[pq.top()->byte], std::move(const_cast<std::unique_ptr<Node>&>(pq.top())), nullptr);
        pq = decltype(pq)();
        pq.push(std::move(root));
    }

    while (pq.size() > 1) {
        auto a = std::move(const_cast<std::unique_ptr<Node>&>(pq.top())); pq.pop();
        auto b = std::move(const_cast<std::unique_ptr<Node>&>(pq.top())); pq.pop();
        pq.push(std::make_unique<Node>(a->freq + b->freq, std::move(a), std::move(b)));
    }

    auto root = std::move(const_cast<std::unique_ptr<Node>&>(pq.top()));

    // Construir códigos
    std::vector<std::string> codes(256);
    buildCodes(root.get(), "", codes);

    // Crear archivo de salida
    std::string outPath = outputPath.empty() ? (inputPath + ".HUB") : outputPath;
    std::ofstream output(outPath, std::ios::binary);
    if (!output) {
        std::cerr << "Error: No se pudo crear el archivo: " << outPath << "\n";
        return false;
    }

    // Escribir cabecera
    output.write("HUB1", 4); // Magic
    writeLE(output, originalSize, 8); // Tamanio original
    writeLE(output, symbolCount, 2); // Numero de simbolos

    // Escribir tabla de frecuencias
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            output.put(static_cast<char>(i));
            writeLE(output, freq[i], 8);
        }
    }

    // Codificar datos
    uint8_t buffer = 0;
    int bits = 0;
    uint64_t totalBits = 0;

    for (uint8_t b : data) {
        const std::string& code = codes[b];
        for (char bit : code) {
            buffer <<= 1;
            if (bit == '1') buffer |= 1;
            bits++;
            totalBits++;

            if (bits == 8) {
                output.put(static_cast<char>(buffer));
                buffer = 0;
                bits = 0;
            }
        }
    }

    if (bits > 0) {
        buffer <<= (8 - bits);
        output.put(static_cast<char>(buffer));
    }

    // Escribir numero total de bits
    writeLE(output, totalBits, 8);
    output.close();

    // Mostrar resultados
    uint64_t compressedSize = std::filesystem::file_size(outPath);
    double ratio = (1.0 - static_cast<double>(compressedSize) / originalSize) * 100.0;
    
    std::cout << "Compresion completada exitosamente!\n";
    std::cout << "Archivo comprimido: " << compressedSize << " bytes\n";
    std::cout << "Ratio de compresion: " << std::fixed << std::setprecision(2) << ratio << "%\n";
    std::cout << "Guardado como: " << outPath << "\n";

    return true;
}

bool HuffmanCompressor::decompress(const std::string& inputPath, const std::string& outputPath) {
    std::cout << "\nIniciando descompresion...\n";

    // Abrir archivo de entrada
    std::ifstream input(inputPath, std::ios::binary);
    if (!input) {
        std::cerr << "Error: No se pudo abrir el archivo: " << inputPath << "\n";
        return false;
    }

    // Verificar magic
    char magic[4];
    input.read(magic, 4);
    if (input.gcount() != 4 || std::string(magic, 4) != "HUB1") {
        std::cerr << "Error: Formato de archivo invalido.\n";
        return false;
    }

    // Leer cabecera
    uint64_t originalSize = readLE(input, 8);
    uint64_t symbolCount = readLE(input, 2);

    if (originalSize == UINT64_MAX || symbolCount == UINT64_MAX) {
        std::cerr << "Error: Cabecera del archivo corrupta.\n";
        return false;
    }

    std::cout << "Tamanio original: " << originalSize << " bytes\n";

    // Leer tabla de frecuencias
    std::array<uint64_t, 256> freq{};
    for (uint64_t i = 0; i < symbolCount; ++i) {
        int byte = input.get();
        if (byte == EOF) {
            std::cerr << "Error: Tabla de frecuencias truncada.\n";
            return false;
        }
        uint64_t frequency = readLE(input, 8);
        if (frequency == UINT64_MAX) {
            std::cerr << "Error: Tabla de frecuencias corrupta.\n";
            return false;
        }
        freq[static_cast<uint8_t>(byte)] = frequency;
    }

    // Reconstruir arbol de Huffman
    std::priority_queue<std::unique_ptr<Node>, std::vector<std::unique_ptr<Node>>, NodeComparator> pq;

    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            pq.push(std::make_unique<Node>(freq[i], i));
        }
    }

    if (pq.empty()) {
        std::cerr << "Error: No hay simbolos en el archivo.\n";
        return false;
    }

    if (pq.size() == 1) {
        // Caso especial: solo un simbolo unico
        auto root = std::make_unique<Node>(freq[pq.top()->byte], std::move(const_cast<std::unique_ptr<Node>&>(pq.top())), nullptr);
        pq = decltype(pq)();
        pq.push(std::move(root));
    }

    while (pq.size() > 1) {
        auto a = std::move(const_cast<std::unique_ptr<Node>&>(pq.top())); pq.pop();
        auto b = std::move(const_cast<std::unique_ptr<Node>&>(pq.top())); pq.pop();
        pq.push(std::make_unique<Node>(a->freq + b->freq, std::move(a), std::move(b)));
    }

    auto root = std::move(const_cast<std::unique_ptr<Node>&>(pq.top()));

    // Obtener posicion actual y leer numero total de bits del final
    std::streampos dataStart = input.tellg();
    input.seekg(-8, std::ios::end);
    uint64_t totalBits = readLE(input, 8);
    input.seekg(dataStart);

    // Crear archivo de salida
    std::string outPath = outputPath.empty() ? (inputPath + ".txt") : outputPath;
    std::ofstream output(outPath, std::ios::binary);
    if (!output) {
        std::cerr << "Error: No se pudo crear el archivo: " << outPath << "\n";
        return false;
    }

    // Decodificar datos
    const Node* current = root.get();
    uint64_t bitsRead = 0;
    uint64_t bytesProduced = 0;
    int byte;

    while ((byte = input.get()) != EOF && bytesProduced < originalSize) {
        // Verificar si llegamos a los ultimos 8 bytes (numero total de bits)
        std::streampos currentPos = input.tellg();
        input.seekg(0, std::ios::end);
        std::streampos endPos = input.tellg();
        input.seekg(currentPos);
        
        if (currentPos >= endPos - std::streamoff(8)) break;

        for (int bit = 7; bit >= 0 && bitsRead < totalBits && bytesProduced < originalSize; --bit) {
            int bitValue = (byte >> bit) & 1;
            current = bitValue ? current->right.get() : current->left.get();
            bitsRead++;

            if (current->byte >= 0) {
                output.put(static_cast<char>(current->byte));
                bytesProduced++;
                current = root.get();
            }
        }
    }

    output.close();
    input.close();

    if (bytesProduced != originalSize) {
        std::cerr << "Advertencia: Tamanio descomprimido (" << bytesProduced 
                  << ") no coincide con el esperado (" << originalSize << ").\n";
    }

    std::cout << "Descompresion completada exitosamente!\n";
    std::cout << "Bytes descomprimidos: " << bytesProduced << "\n";
    std::cout << "Guardado como: " << outPath << "\n";

    return true;
}