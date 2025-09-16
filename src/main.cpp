#include "huffman.hpp"
#include <iostream>
#include <string>

void mostrarBanner() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              🗜️  HUFFMAN COMPRESSION TOOL  🗜️               ║\n";
    std::cout << "║                     Versión Simplificada                    ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void mostrarMenu() {
    std::cout << "🎯 === MENÚ PRINCIPAL ===\n\n";
    std::cout << "1️⃣  Comprimir archivo\n";
    std::cout << "2️⃣  Descomprimir archivo .HUB\n";
    std::cout << "3️⃣  Mostrar ayuda\n";
    std::cout << "4️⃣  Salir\n\n";
}

void menuComprimir() {
    std::cout << "\n🗜️  === COMPRIMIR ARCHIVO ===\n";
    std::cout << "📁 Ingrese la ruta del archivo a comprimir: ";
    
    std::string ruta;
    std::getline(std::cin, ruta);
    
    if (ruta.empty()) {
        std::cout << "❌ Ruta vacía. Operación cancelada.\n";
        return;
    }

    // Remover comillas si están presentes
    if (ruta.front() == '"' && ruta.back() == '"') {
        ruta = ruta.substr(1, ruta.length() - 2);
    }

    if (HuffmanCompressor::compress(ruta)) {
        std::cout << "\n🎉 ¡Compresión exitosa!\n";
    } else {
        std::cout << "\n💥 Error durante la compresión.\n";
    }
}

void menuDescomprimir() {
    std::cout << "\n📤 === DESCOMPRIMIR ARCHIVO ===\n";
    std::cout << "📁 Ingrese la ruta del archivo .HUB a descomprimir: ";
    
    std::string ruta;
    std::getline(std::cin, ruta);
    
    if (ruta.empty()) {
        std::cout << "❌ Ruta vacía. Operación cancelada.\n";
        return;
    }

    // Remover comillas si están presentes
    if (ruta.front() == '"' && ruta.back() == '"') {
        ruta = ruta.substr(1, ruta.length() - 2);
    }

    if (HuffmanCompressor::decompress(ruta)) {
        std::cout << "\n🎉 ¡Descompresión exitosa!\n";
    } else {
        std::cout << "\n💥 Error durante la descompresión.\n";
    }
}

void mostrarAyuda() {
    std::cout << "\n❓ === AYUDA ===\n\n";
    std::cout << "📝 Instrucciones de uso:\n\n";
    std::cout << "🗜️  COMPRESIÓN:\n";
    std::cout << "   • Seleccione la opción 1 del menú\n";
    std::cout << "   • Ingrese la ruta completa del archivo\n";
    std::cout << "   • El archivo comprimido se guardará con extensión .HUB\n\n";
    
    std::cout << "📤 DESCOMPRESIÓN:\n";
    std::cout << "   • Seleccione la opción 2 del menú\n";
    std::cout << "   • Ingrese la ruta del archivo .HUB\n";
    std::cout << "   • El archivo se descomprimirá con extensión .txt\n\n";
    
    std::cout << "💡 CONSEJOS:\n";
    std::cout << "   • Use comillas si la ruta contiene espacios\n";
    std::cout << "   • Los archivos de texto comprimen mejor\n";
    std::cout << "   • Archivos ya comprimidos (ZIP, JPG) pueden crecer\n\n";
}

int main() {
    std::cout << "Iniciando Huffman Compression Tool...\n";

    while (true) {
        mostrarBanner();
        mostrarMenu();
        
        std::cout << "🔸 Seleccione una opción (1-4): ";
        std::string opcion;
        std::getline(std::cin, opcion);
        
        if (opcion == "1") {
            menuComprimir();
        } else if (opcion == "2") {
            menuDescomprimir();
        } else if (opcion == "3") {
            mostrarAyuda();
        } else if (opcion == "4") {
            std::cout << "\n👋 ¡Gracias por usar Huffman Compression Tool!\n";
            std::cout << "🚪 Saliendo...\n";
            break;
        } else {
            std::cout << "\n❌ Opción inválida. Por favor seleccione 1-4.\n";
        }
        
        if (opcion != "4") {
            std::cout << "\nPresione Enter para continuar...";
            std::cin.get();
        }
    }

    return 0;
}
