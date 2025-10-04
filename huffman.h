#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>

using namespace std;

/*
 * ================================================================================================
 * ALGORITMO DE COMPRESIÓN HUFFMAN - METODOLOGÍA COMPLETA
 * ================================================================================================
 * 
 * El algoritmo de Huffman es un método de compresión sin pérdida que utiliza códigos de longitud 
 * variable para representar caracteres. Los caracteres más frecuentes reciben códigos más cortos,
 * mientras que los menos frecuentes reciben códigos más largos.
 * 
 * PROCESO DE COMPRESIÓN:
 * 1. Análisis de Frecuencias: Contar la frecuencia de cada carácter en el texto
 * 2. Construcción del Árbol: Crear un árbol binario donde las hojas son caracteres
 * 3. Generación de Códigos: Asignar códigos binarios basados en la posición en el árbol
 * 4. Codificación: Reemplazar cada carácter por su código Huffman
 * 5. Serialización: Guardar el árbol y los datos codificados en el archivo .hub
 * 
 * PROCESO DE DESCOMPRESIÓN:
 * 1. Deserialización: Reconstruir el árbol Huffman desde el archivo .hub
 * 2. Decodificación: Usar el árbol para convertir códigos binarios a caracteres
 * 3. Reconstrucción: Regenerar el texto original completo
 * 
 * ================================================================================================
 */

/**
 * Nodo del árbol binario Huffman
 * 
 * Cada nodo puede ser:
 * - Nodo hoja: contiene un carácter y su frecuencia (left=null, right=null)
 * - Nodo interno: contiene la suma de frecuencias de sus hijos (character=null)
 * 
 * El árbol se construye de abajo hacia arriba, combinando nodos con menor frecuencia
 */
struct HuffmanNode {
    char character;      // Carácter almacenado (solo en hojas)
    int frequency;       // Frecuencia del carácter o suma de frecuencias
    HuffmanNode* left;   // Puntero al hijo izquierdo (código '0')
    HuffmanNode* right;  // Puntero al hijo derecho (código '1')
    
    // Constructor para nodos hoja (contienen un carácter)
    HuffmanNode(char ch, int freq) : character(ch), frequency(freq), left(nullptr), right(nullptr) {}
    
    // Constructor para nodos internos (no contienen carácter)
    HuffmanNode(int freq) : character('\0'), frequency(freq), left(nullptr), right(nullptr) {}
};

/**
 * Comparador para la cola de prioridad (min-heap)
 * 
 * La cola de prioridad necesita ordenar los nodos por frecuencia ascendente.
 * Los nodos con menor frecuencia tienen mayor prioridad para ser procesados primero.
 * Esto es fundamental para construir el árbol Huffman óptimo.
 */
struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;  // Min-heap: menor frecuencia = mayor prioridad
    }
};

/**
 * Clase principal del compresor Huffman
 * 
 * Maneja todo el proceso de compresión y descompresión:
 * - Construcción del árbol Huffman
 * - Generación de códigos binarios
 * - Serialización/deserialización del árbol
 * - Codificación/decodificación de datos
 */
class HuffmanCompressor {
private:
    HuffmanNode* root;                              // Raíz del árbol Huffman
    unordered_map<char, string> huffmanCodes;       // Mapeo: carácter -> código binario
    unordered_map<char, int> frequency;             // Mapeo: carácter -> frecuencia
    
    /**
     * MÉTODO 1: Análisis de Frecuencias
     * Cuenta la frecuencia de aparición de cada carácter en el texto.
     * Esta información es crucial para construir el árbol Huffman óptimo.
     */
    void calculateFrequency(const string& text);
    
    /**
     * MÉTODO 2: Construcción del Árbol Huffman
     * Utiliza una cola de prioridad (min-heap) para construir el árbol:
     * 1. Crear nodos hoja para cada carácter con su frecuencia
     * 2. Mientras haya más de un nodo en la cola:
     *    - Extraer los dos nodos con menor frecuencia
     *    - Crear un nodo padre con la suma de sus frecuencias
     *    - Insertar el nodo padre de vuelta en la cola
     * 3. El último nodo restante es la raíz del árbol
     */
    HuffmanNode* buildHuffmanTree();
    
    /**
     * MÉTODO 3: Generación de Códigos
     * Recorre el árbol Huffman para asignar códigos binarios:
     * - Camino hacia la izquierda: agregar '0' al código
     * - Camino hacia la derecha: agregar '1' al código
     * - Al llegar a una hoja: guardar el código completo para ese carácter
     */
    void generateCodes(HuffmanNode* node, string code);
    
    /**
     * MÉTODO 4: Serialización del Árbol
     * Convierte el árbol en una secuencia de bytes para guardarlo en archivo:
     * - '0': marca un nodo nulo
     * - '1': marca un nodo hoja (seguido del carácter)
     * - '2': marca un nodo interno (seguido de sus hijos serializados)
     */
    void serializeTree(HuffmanNode* node, ofstream& outFile);
    
    /**
     * MÉTODO 5: Deserialización del Árbol
     * Reconstruye el árbol Huffman desde la secuencia de bytes guardada.
     * Lee los marcadores y reconstruye recursivamente la estructura del árbol.
     */
    HuffmanNode* deserializeTree(ifstream& inFile);
    
    /**
     * MÉTODO 6: Liberación de Memoria
     * Elimina recursivamente todos los nodos del árbol para evitar memory leaks.
     */
    void deleteTree(HuffmanNode* node);
    
public:
    HuffmanCompressor();
    ~HuffmanCompressor();
    
    /**
     * PROCESO COMPLETO DE COMPRESIÓN
     * 1. Lee el archivo original
     * 2. Calcula frecuencias de caracteres
     * 3. Construye el árbol Huffman
     * 4. Genera códigos binarios
     * 5. Serializa el árbol en el archivo .hub
     * 6. Codifica el texto usando los códigos Huffman
     * 7. Guarda los datos codificados como bits comprimidos
     */
    bool compressFile(const string& inputFile, const string& outputFile);
    
    /**
     * PROCESO COMPLETO DE DESCOMPRESIÓN
     * 1. Lee el archivo .hub comprimido
     * 2. Deserializa el árbol Huffman
     * 3. Lee los datos codificados bit por bit
     * 4. Usa el árbol para decodificar cada secuencia binaria
     * 5. Reconstruye el texto original
     * 6. Guarda el resultado en un archivo .txt
     */
    bool decompressFile(const string& inputFile, const string& outputFile);
    
    /**
     * Muestra estadísticas de compresión:
     * - Tamaño original vs comprimido
     * - Porcentaje de compresión
     * - Espacio ahorrado
     */
    void displayCompressionStats(const string& originalFile, const string& compressedFile);
};

#endif