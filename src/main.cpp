#include "huffman.hpp"
#include <iostream>
#include <string>

static void menuComprimir() {
    std::string ruta;
    std::cout << "Ruta archivo a comprimir: " << std::flush;
    std::getline(std::cin, ruta);
    if(ruta.empty()) {
        std::cout << "Ruta vacía.\n";
        return;
    }
    if(HuffmanCompressor::compress(ruta)) {
        std::cout << "Comprimido OK.\n";
    } else {
        std::cout << "Error al comprimir.\n";
    }
}

static void menuDescomprimir() {
    std::string ruta;
    std::cout << "Ruta archivo .HUB a descomprimir: " << std::flush;
    std::getline(std::cin, ruta);
    if(ruta.empty()) {
        std::cout << "Ruta vacía.\n";
        return;
    }
    if(HuffmanCompressor::decompress(ruta)) {
        std::cout << "Descomprimido OK.\n";
    } else {
        std::cout << "Error al descomprimir.\n";
    }
}

int main() {
    while(true) {
        std::cout << "\n=== Menu Principal ===\n";
        std::cout << "1 - Comprimir\n";
        std::cout << "2 - Descomprimir\n";
        std::cout << "3 - Salir\n";
    std::cout << "Opcion: " << std::flush;
        std::string op;
        if(!std::getline(std::cin, op)) break;
        if(op=="1") {
            menuComprimir();
        } else if(op=="2") {
            menuDescomprimir();
        } else if(op=="3") {
            std::cout << "Saliendo.\n";
            break;
        } else {
            std::cout << "Opcion inválida.\n";
        }
    }
    return 0;
}
