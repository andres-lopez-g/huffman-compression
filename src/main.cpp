#include "huffman.hpp"
#include <iostream>
#include <string>

void mostrarBanner() {
    std::cout << "\n";
    std::cout << "================================================================\n";
    std::cout << "              HUFFMAN COMPRESSION TOOL                         \n";
    std::cout << "                   Version Simplificada                        \n";
    std::cout << "================================================================\n";
    std::cout << "\n";
}

void mostrarMenu() {
    std::cout << "=== MENU PRINCIPAL ===\n\n";
    std::cout << "1. Comprimir archivo\n";
    std::cout << "2. Descomprimir archivo .HUB\n";
    std::cout << "3. Mostrar ayuda\n";
    std::cout << "4. Salir\n\n";
}

void menuComprimir() {
    std::cout << "\n=== COMPRIMIR ARCHIVO ===\n";
    std::cout << "Ingrese la ruta del archivo a comprimir: ";
    
    std::string ruta;
    std::getline(std::cin, ruta);
    
    if (ruta.empty()) {
        std::cout << "Error: Ruta vacia. Operacion cancelada.\n";
        return;
    }

    // Remover comillas si estan presentes
    if (ruta.front() == '"' && ruta.back() == '"') {
        ruta = ruta.substr(1, ruta.length() - 2);
    }

    // Preguntar por nombre personalizado
    std::cout << "\nDesea especificar un nombre para el archivo comprimido? (s/n): ";
    std::string respuesta;
    std::getline(std::cin, respuesta);
    
    std::string nombreSalida = "";
    if (respuesta == "s" || respuesta == "S") {
        std::cout << "Ingrese el nombre del archivo de salida (sin extension): ";
        std::getline(std::cin, nombreSalida);
        
        if (!nombreSalida.empty()) {
            // Agregar extension .HUB si no la tiene
            if (nombreSalida.length() < 4 || nombreSalida.substr(nombreSalida.length() - 4) != ".HUB") {
                nombreSalida += ".HUB";
            }
        }
    }

    if (HuffmanCompressor::compress(ruta, nombreSalida)) {
        std::cout << "\nCompresion exitosa!\n";
    } else {
        std::cout << "\nError durante la compresion.\n";
    }
}

void menuDescomprimir() {
    std::cout << "\n=== DESCOMPRIMIR ARCHIVO ===\n";
    std::cout << "Ingrese la ruta del archivo .HUB a descomprimir: ";
    
    std::string ruta;
    std::getline(std::cin, ruta);
    
    if (ruta.empty()) {
        std::cout << "Error: Ruta vacia. Operacion cancelada.\n";
        return;
    }

    // Remover comillas si estan presentes
    if (ruta.front() == '"' && ruta.back() == '"') {
        ruta = ruta.substr(1, ruta.length() - 2);
    }

    // Preguntar por nombre personalizado
    std::cout << "\nDesea especificar un nombre para el archivo descomprimido? (s/n): ";
    std::string respuesta;
    std::getline(std::cin, respuesta);
    
    std::string nombreSalida = "";
    if (respuesta == "s" || respuesta == "S") {
        std::cout << "Ingrese el nombre del archivo de salida (con extension): ";
        std::getline(std::cin, nombreSalida);
    }

    if (HuffmanCompressor::decompress(ruta, nombreSalida)) {
        std::cout << "\nDescompresion exitosa!\n";
    } else {
        std::cout << "\nError durante la descompresion.\n";
    }
}

void mostrarAyuda() {
    std::cout << "\n=== AYUDA ===\n\n";
    std::cout << "Instrucciones de uso:\n\n";
    std::cout << "COMPRESION:\n";
    std::cout << "   - Seleccione la opcion 1 del menu\n";
    std::cout << "   - Ingrese la ruta completa del archivo\n";
    std::cout << "   - Opcionalmente, especifique un nombre personalizado\n";
    std::cout << "   - El archivo comprimido se guardara con extension .HUB\n\n";
    
    std::cout << "DESCOMPRESION:\n";
    std::cout << "   - Seleccione la opcion 2 del menu\n";
    std::cout << "   - Ingrese la ruta del archivo .HUB\n";
    std::cout << "   - Opcionalmente, especifique un nombre personalizado\n";
    std::cout << "   - El archivo se descomprimira con su extension original\n\n";
    
    std::cout << "CONSEJOS:\n";
    std::cout << "   - Use comillas si la ruta contiene espacios\n";
    std::cout << "   - Los archivos de texto comprimen mejor\n";
    std::cout << "   - Archivos ya comprimidos (ZIP, JPG) pueden crecer\n";
    std::cout << "   - Puede especificar rutas relativas o absolutas\n\n";
}

int main() {
    std::cout << "Iniciando Huffman Compression Tool...\n";

    while (true) {
        mostrarBanner();
        mostrarMenu();
        
        std::cout << "Seleccione una opcion (1-4): ";
        std::string opcion;
        std::getline(std::cin, opcion);
        
        if (opcion == "1") {
            menuComprimir();
        } else if (opcion == "2") {
            menuDescomprimir();
        } else if (opcion == "3") {
            mostrarAyuda();
        } else if (opcion == "4") {
            std::cout << "\nGracias por usar Huffman Compression Tool!\n";
            std::cout << "Saliendo...\n";
            break;
        } else {
            std::cout << "\nOpcion invalida. Por favor seleccione 1-4.\n";
        }
        
        if (opcion != "4") {
            std::cout << "\nPresione Enter para continuar...";
            std::cin.get();
        }
    }

    return 0;
}
