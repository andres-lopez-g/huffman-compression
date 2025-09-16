#include "huffman.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void showMainMenu() {
    cout << "\n========================================" << endl;
    cout << "      COMPRESOR HUFFMAN por Andres J Lopez" << endl;
    cout << "========================================" << endl;
    cout << "Escoja una opcion:" << endl;
    cout << "1. Comprimir archivo" << endl;
    cout << "2. Descomprimir archivo" << endl;
    cout << "3. Salir" << endl;
    cout << "========================================" << endl;
    cout << "Opcion: ";
}

string getFilePath(const string& action) {
    string path;
    cout << "\n========================================" << endl;
    cout << "      " << action << endl;
    cout << "========================================" << endl;
    cout << "Ingrese la ruta completa del archivo:" << endl;
    cout << "Ejemplo: C:\\ruta\\al\\archivo.txt" << endl;
    cout << "Ruta: ";
    
    cin.ignore(); // Limpiar el buffer
    getline(cin, path);
    
    return path;
}

string generateOutputPath(const string& inputPath, const string& extension) {
    size_t lastDot = inputPath.find_last_of('.');
    size_t lastSlash = inputPath.find_last_of("\\/");
    
    string baseName;
    if (lastDot != string::npos && (lastSlash == string::npos || lastDot > lastSlash)) {
        baseName = inputPath.substr(0, lastDot);
    } else {
        baseName = inputPath;
    }
    
    return baseName + extension;
}

void handleCompress(HuffmanCompressor& compressor) {
    string inputFile = getFilePath("COMPRIMIR ARCHIVO");
    
    if (inputFile.empty()) {
        cout << "Error: Ruta vacia." << endl;
        return;
    }
    
    string outputFile = generateOutputPath(inputFile, ".hub");
    
    cout << "\nArchivo de entrada: " << inputFile << endl;
    cout << "Archivo de salida: " << outputFile << endl;
    cout << "\nProcesando..." << endl;
    
    if (compressor.compressFile(inputFile, outputFile)) {
        cout << "\n¡Compresion completada exitosamente!" << endl;
        cout << "Archivo comprimido guardado como: " << outputFile << endl;
    } else {
        cout << "\nError durante la compresión." << endl;
    }
    
    cout << "\nPresione Enter para continuar...";
    cin.ignore();
    cin.get();
}

void handleDecompress(HuffmanCompressor& compressor) {
    string inputFile = getFilePath("DESCOMPRIMIR ARCHIVO");
    
    if (inputFile.empty()) {
        cout << "Error: Ruta vacia." << endl;
        return;
    }
    
    string outputFile = generateOutputPath(inputFile, ".txt");
    
    cout << "\nArchivo de entrada: " << inputFile << endl;
    cout << "Archivo de salida: " << outputFile << endl;
    cout << "\nProcesando..." << endl;
    
    if (compressor.decompressFile(inputFile, outputFile)) {
        cout << "\n¡Descompresion completada exitosamente!" << endl;
        cout << "Archivo descomprimido guardado como: " << outputFile << endl;
    } else {
        cout << "\nError durante la descompresion." << endl;
    }
    
    cout << "\nPresione Enter para continuar...";
    cin.ignore();
    cin.get();
}

int main() {
    HuffmanCompressor compressor;
    int option;
    bool running = true;
    
    cout << "Inicializando Compresor Huffman..." << endl;
    
    while (running) {
        clearScreen();
        showMainMenu();
        
        if (!(cin >> option)) {
            cout << "Error: Ingrese un numero valido." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Presione Enter para continuar...";
            cin.get();
            continue;
        }
        
        switch (option) {
            case 1:
                clearScreen();
                handleCompress(compressor);
                break;
                
            case 2:
                clearScreen();
                handleDecompress(compressor);
                break;
                
            case 3:
                cout << "\n¡Gracias por usar el Compresor Huffman!" << endl;
                cout << "Saliendo del programa..." << endl;
                running = false;
                break;
                
            default:
                cout << "\nOpción invalida. Por favor seleccione 1, 2 o 3." << endl;
                cout << "Presione Enter para continuar...";
                cin.ignore();
                cin.get();
                break;
        }
    }
    
    return 0;
}