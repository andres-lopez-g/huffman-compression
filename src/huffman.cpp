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
    std::cout << "\n🗜️  Iniciando compresión...\n";
    
    // Abrir archivo de entrada
    std::ifstream input(inputPath, std::ios::binary);
    if (!input) {
        std::cerr << "❌ Error: No se pudo abrir el archivo: " << inputPath << "\n";
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
        std::cerr << "❌ Error: El archivo está vacío.\n";
        return false;
    }

    uint64_t originalSize = data.size();
    std::cout << "📊 Tamaño original: " << originalSize << " bytes\n";

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
        // Caso especial: solo un símbolo único
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
        std::cerr << "❌ Error: No se pudo crear el archivo: " << outPath << "\n";
        return false;
    }

    // Escribir cabecera
    output.write("HUB1", 4); // Magic
    writeLE(output, originalSize, 8); // Tamaño original
    writeLE(output, symbolCount, 2); // Número de símbolos

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

    // Escribir número total de bits
    writeLE(output, totalBits, 8);
    output.close();

    // Mostrar resultados
    uint64_t compressedSize = std::filesystem::file_size(outPath);
    double ratio = (1.0 - static_cast<double>(compressedSize) / originalSize) * 100.0;
    
    std::cout << "✅ Compresión completada exitosamente!\n";
    std::cout << "📦 Archivo comprimido: " << compressedSize << " bytes\n";
    std::cout << "📈 Ratio de compresión: " << std::fixed << std::setprecision(2) << ratio << "%\n";
    std::cout << "💾 Guardado como: " << outPath << "\n";

    return true;
}

bool HuffmanCompressor::decompress(const std::string& inputPath, const std::string& outputPath) {
    std::cout << "\n📤 Iniciando descompresión...\n";

    // Abrir archivo de entrada
    std::ifstream input(inputPath, std::ios::binary);
    if (!input) {
        std::cerr << "❌ Error: No se pudo abrir el archivo: " << inputPath << "\n";
        return false;
    }

    // Verificar magic
    char magic[4];
    input.read(magic, 4);
    if (input.gcount() != 4 || std::string(magic, 4) != "HUB1") {
        std::cerr << "❌ Error: Formato de archivo inválido.\n";
        return false;
    }

    // Leer cabecera
    uint64_t originalSize = readLE(input, 8);
    uint64_t symbolCount = readLE(input, 2);

    if (originalSize == UINT64_MAX || symbolCount == UINT64_MAX) {
        std::cerr << "❌ Error: Cabecera del archivo corrupta.\n";
        return false;
    }

    std::cout << "📊 Tamaño original: " << originalSize << " bytes\n";

    // Leer tabla de frecuencias
    std::array<uint64_t, 256> freq{};
    for (uint64_t i = 0; i < symbolCount; ++i) {
        int byte = input.get();
        if (byte == EOF) {
            std::cerr << "❌ Error: Tabla de frecuencias truncada.\n";
            return false;
        }
        uint64_t frequency = readLE(input, 8);
        if (frequency == UINT64_MAX) {
            std::cerr << "❌ Error: Tabla de frecuencias corrupta.\n";
            return false;
        }
        freq[static_cast<uint8_t>(byte)] = frequency;
    }

    // Reconstruir árbol de Huffman
    std::priority_queue<std::unique_ptr<Node>, std::vector<std::unique_ptr<Node>>, NodeComparator> pq;

    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            pq.push(std::make_unique<Node>(freq[i], i));
        }
    }

    if (pq.empty()) {
        std::cerr << "❌ Error: No hay símbolos en el archivo.\n";
        return false;
    }

    if (pq.size() == 1) {
        // Caso especial: solo un símbolo único
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

    // Obtener posición actual y leer número total de bits del final
    std::streampos dataStart = input.tellg();
    input.seekg(-8, std::ios::end);
    uint64_t totalBits = readLE(input, 8);
    input.seekg(dataStart);

    // Crear archivo de salida
    std::string outPath = outputPath.empty() ? (inputPath + ".txt") : outputPath;
    std::ofstream output(outPath, std::ios::binary);
    if (!output) {
        std::cerr << "❌ Error: No se pudo crear el archivo: " << outPath << "\n";
        return false;
    }

    // Decodificar datos
    const Node* current = root.get();
    uint64_t bitsRead = 0;
    uint64_t bytesProduced = 0;
    int byte;

    while ((byte = input.get()) != EOF && bytesProduced < originalSize) {
        // Verificar si llegamos a los últimos 8 bytes (número total de bits)
        std::streampos currentPos = input.tellg();
        input.seekg(0, std::ios::end);
        std::streampos endPos = input.tellg();
        input.seekg(currentPos);
        
        if (currentPos >= endPos - 8) break;

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
        std::cerr << "⚠️  Advertencia: Tamaño descomprimido (" << bytesProduced 
                  << ") no coincide con el esperado (" << originalSize << ").\n";
    }

    std::cout << "✅ Descompresión completada exitosamente!\n";
    std::cout << "📦 Bytes descomprimidos: " << bytesProduced << "\n";
    std::cout << "💾 Guardado como: " << outPath << "\n";

    return true;
}

bool HuffmanCompressor::compress(const std::wstring& inputPath, const std::wstring& outputPath) {
    WindowsConsole::setColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::wcout << L"Iniciando compresión...\n";
    WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    if (!fileExists(inputPath)) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error: No se pudo encontrar el archivo: " << inputPath << L"\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    // Read input file
    HANDLE hFile = CreateFileW(inputPath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error al abrir archivo: " << getLastError() << L"\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    LARGE_INTEGER fileSize;
    GetFileSizeEx(hFile, &fileSize);
    uint64_t originalSize = static_cast<uint64_t>(fileSize.QuadPart);

    if (originalSize == 0) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error: El archivo está vacío.\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        CloseHandle(hFile);
        return false;
    }

    std::vector<uint8_t> data(originalSize);
    DWORD bytesRead;
    ReadFile(hFile, data.data(), static_cast<DWORD>(originalSize), &bytesRead, nullptr);
    CloseHandle(hFile);

    // Build frequency table
    std::array<uint64_t, 256> freq{};
    for (uint8_t byte : data) {
        freq[byte]++;
    }

    // Build Huffman tree
    std::priority_queue<std::unique_ptr<Node>, std::vector<std::unique_ptr<Node>>, NodeComparator> pq;
    int symbolCount = 0;
    
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            pq.push(std::make_unique<Node>(freq[i], i));
            symbolCount++;
        }
    }

    if (symbolCount == 1) {
        // Special case: only one unique symbol
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

    // Build codes
    std::vector<std::string> codes(256);
    buildCodes(root.get(), "", codes);

    // Create output file
    std::wstring outPath = outputPath.empty() ? (inputPath + L".HUB") : outputPath;
    
    std::vector<uint8_t> output;
    output.reserve(originalSize); // Reserve some space

    // Write header
    output.insert(output.end(), {'H', 'U', 'B', '1'}); // Magic
    writeLE(output, originalSize, 8); // Original size
    writeLE(output, symbolCount, 2); // Symbol count

    // Write frequency table
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            output.push_back(static_cast<uint8_t>(i));
            writeLE(output, freq[i], 8);
        }
    }

    // Encode data
    uint8_t buffer = 0;
    int bits = 0;
    uint64_t totalBits = 0;
    
    for (uint8_t byte : data) {
        const std::string& code = codes[byte];
        for (char bit : code) {
            buffer <<= 1;
            if (bit == '1') buffer |= 1;
            bits++;
            totalBits++;
            
            if (bits == 8) {
                output.push_back(buffer);
                buffer = 0;
                bits = 0;
            }
        }
    }
    
    if (bits > 0) {
        buffer <<= (8 - bits);
        output.push_back(buffer);
    }

    // Write total bits count
    writeLE(output, totalBits, 8);

    // Write output file
    hFile = CreateFileW(outPath.c_str(), GENERIC_WRITE, 0, nullptr, 
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error al crear archivo de salida: " << getLastError() << L"\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    DWORD bytesWritten;
    WriteFile(hFile, output.data(), static_cast<DWORD>(output.size()), &bytesWritten, nullptr);
    CloseHandle(hFile);

    // Display results
    WindowsConsole::setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::wcout << L"✓ Compresión completada exitosamente!\n";
    std::wcout << L"  Archivo original: " << originalSize << L" bytes\n";
    std::wcout << L"  Archivo comprimido: " << output.size() << L" bytes\n";
    
    double ratio = (1.0 - static_cast<double>(output.size()) / originalSize) * 100.0;
    std::wcout << L"  Ratio de compresión: " << std::fixed << std::setprecision(2) << ratio << L"%\n";
    std::wcout << L"  Archivo guardado: " << outPath << L"\n";
    WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    return true;
}

bool HuffmanCompressor::decompress(const std::wstring& inputPath, const std::wstring& outputPath) {
    WindowsConsole::setColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::wcout << L"Iniciando descompresión...\n";
    WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    if (!fileExists(inputPath)) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error: No se pudo encontrar el archivo: " << inputPath << L"\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    // Read input file
    HANDLE hFile = CreateFileW(inputPath.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error al abrir archivo: " << getLastError() << L"\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    LARGE_INTEGER fileSize;
    GetFileSizeEx(hFile, &fileSize);
    std::vector<uint8_t> data(static_cast<size_t>(fileSize.QuadPart));
    
    DWORD bytesRead;
    ReadFile(hFile, data.data(), static_cast<DWORD>(data.size()), &bytesRead, nullptr);
    CloseHandle(hFile);

    // Parse header
    size_t offset = 0;
    
    // Check magic
    if (data.size() < 4 || std::string(data.begin(), data.begin() + 4) != "HUB1") {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error: Formato de archivo inválido.\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }
    offset += 4;

    uint64_t originalSize = readLE(data.data(), offset, 8, data.size());
    uint64_t symbolCount = readLE(data.data(), offset, 2, data.size());

    if (originalSize == UINT64_MAX || symbolCount == UINT64_MAX) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error: Cabecera del archivo corrupta.\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    // Read frequency table
    std::array<uint64_t, 256> freq{};
    for (uint64_t i = 0; i < symbolCount; ++i) {
        if (offset >= data.size()) {
            WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::wcout << L"Error: Tabla de frecuencias truncada.\n";
            WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            return false;
        }
        
        uint8_t byte = data[offset++];
        uint64_t frequency = readLE(data.data(), offset, 8, data.size());
        if (frequency == UINT64_MAX) {
            WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::wcout << L"Error: Tabla de frecuencias corrupta.\n";
            WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            return false;
        }
        freq[byte] = frequency;
    }

    // Rebuild Huffman tree
    std::priority_queue<std::unique_ptr<Node>, std::vector<std::unique_ptr<Node>>, NodeComparator> pq;
    
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            pq.push(std::make_unique<Node>(freq[i], i));
        }
    }

    if (pq.empty()) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error: No hay símbolos en el archivo.\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    if (pq.size() == 1) {
        // Special case: only one unique symbol
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

    // Read total bits from end of file
    if (data.size() < offset + 8) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error: Archivo truncado.\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    size_t bitsOffset = data.size() - 8;
    uint64_t totalBits = readLE(data.data(), bitsOffset, 8, data.size());

    // Decode data
    std::vector<uint8_t> output;
    output.reserve(originalSize);

    const Node* current = root.get();
    uint64_t bitsRead = 0;
    uint64_t bytesProduced = 0;

    for (size_t i = offset; i < data.size() - 8 && bytesProduced < originalSize; ++i) {
        uint8_t byte = data[i];
        
        for (int bit = 7; bit >= 0 && bitsRead < totalBits && bytesProduced < originalSize; --bit) {
            int bitValue = (byte >> bit) & 1;
            current = bitValue ? current->right.get() : current->left.get();
            bitsRead++;

            if (current->byte >= 0) {
                output.push_back(static_cast<uint8_t>(current->byte));
                bytesProduced++;
                current = root.get();
            }
        }
    }

    if (bytesProduced != originalSize) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Advertencia: El tamaño descomprimido (" << bytesProduced 
                   << L") no coincide con el esperado (" << originalSize << L").\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    // Write output file
    std::wstring outPath = outputPath.empty() ? (inputPath + L".txt") : outputPath;
    
    hFile = CreateFileW(outPath.c_str(), GENERIC_WRITE, 0, nullptr,
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::wcout << L"Error al crear archivo de salida: " << getLastError() << L"\n";
        WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    DWORD bytesWritten;
    WriteFile(hFile, output.data(), static_cast<DWORD>(output.size()), &bytesWritten, nullptr);
    CloseHandle(hFile);

    WindowsConsole::setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::wcout << L"✓ Descompresión completada exitosamente!\n";
    std::wcout << L"  Bytes descomprimidos: " << output.size() << L"\n";
    std::wcout << L"  Archivo guardado: " << outPath << L"\n";
    WindowsConsole::setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    return true;
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
