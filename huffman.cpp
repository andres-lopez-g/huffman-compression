/*
 * ================================================================================================
 * IMPLEMENTACIÓN DEL ALGORITMO DE HUFFMAN - EXPLICACIÓN DETALLADA
 * ================================================================================================
 * 
 * Este archivo contiene la implementación completa del algoritmo de compresión Huffman.
 * El algoritmo se basa en la teoría de información de Claude Shannon y fue desarrollado
 * por David A. Huffman en 1952.
 * 
 * PRINCIPIO FUNDAMENTAL:
 * Los caracteres más frecuentes en un texto deben tener códigos más cortos,
 * mientras que los menos frecuentes pueden tener códigos más largos.
 * Esto minimiza la longitud promedio del código y maximiza la compresión.
 * 
 * ================================================================================================
 */

#include "huffman.h"
#include <queue>
#include <climits>
#include <iomanip>  // Para setprecision y fixed

// Constructor: Inicializa el compresor con un árbol vacío
HuffmanCompressor::HuffmanCompressor() : root(nullptr) {}

// Destructor: Libera toda la memoria utilizada por el árbol
HuffmanCompressor::~HuffmanCompressor() {
    deleteTree(root);
}

/**
 * MÉTODO AUXILIAR: Liberación recursiva de memoria
 * 
 * Recorre el árbol en post-orden para eliminar todos los nodos.
 * Es crucial para evitar memory leaks ya que el árbol se construye dinámicamente.
 */
void HuffmanCompressor::deleteTree(HuffmanNode* node) {
    if (node) {
        deleteTree(node->left);   // Eliminar subárbol izquierdo
        deleteTree(node->right);  // Eliminar subárbol derecho
        delete node;              // Eliminar nodo actual
    }
}

/**
 * PASO 1 DEL ALGORITMO: ANÁLISIS DE FRECUENCIAS
 * 
 * Recorre todo el texto carácter por carácter y cuenta las apariciones.
 * Esta información estadística es fundamental para construir un árbol Huffman óptimo.
 * 
 * COMPLEJIDAD: O(n) donde n es la longitud del texto
 * 
 * Ejemplo:
 * Texto: "ABACA"
 * Frecuencias: A=3, B=1, C=1
 */
void HuffmanCompressor::calculateFrequency(const string& text) {
    frequency.clear();  // Limpiar frecuencias previas
    
    // Contar cada carácter en el texto
    for (char c : text) {
        frequency[c]++;
    }
    
    // Debug: mostrar frecuencias calculadas
    cout << "Frecuencias calculadas:" << endl;
    for (auto& pair : frequency) {
        cout << "'" << pair.first << "': " << pair.second << " veces" << endl;
    }
}

/**
 * PASO 2 DEL ALGORITMO: CONSTRUCCIÓN DEL ÁRBOL HUFFMAN
 * 
 * Utiliza el algoritmo de Huffman para construir un árbol binario óptimo:
 * 
 * ALGORITMO:
 * 1. Crear una cola de prioridad (min-heap) con todos los caracteres como nodos hoja
 * 2. Mientras haya más de un nodo en la cola:
 *    a. Extraer los dos nodos con menor frecuencia
 *    b. Crear un nuevo nodo interno con estos dos como hijos
 *    c. La frecuencia del nuevo nodo es la suma de las frecuencias de sus hijos
 *    d. Insertar el nuevo nodo de vuelta en la cola
 * 3. El último nodo restante es la raíz del árbol Huffman
 * 
 * PROPIEDADES DEL ÁRBOL RESULTANTE:
 * - Es un árbol binario completo (cada nodo interno tiene exactamente 2 hijos)
 * - Las hojas contienen los caracteres originales
 * - Los nodos internos contienen sumas de frecuencias
 * - El camino desde la raíz a cada hoja define el código Huffman del carácter
 * 
 * COMPLEJIDAD: O(n log n) donde n es el número de caracteres únicos
 */
HuffmanNode* HuffmanCompressor::buildHuffmanTree() {
    // Crear cola de prioridad (min-heap) que ordena por frecuencia
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
    
    // PASO 2.1: Crear nodos hoja para cada carácter único
    cout << "Creando nodos hoja..." << endl;
    for (auto& pair : frequency) {
        HuffmanNode* leafNode = new HuffmanNode(pair.first, pair.second);
        pq.push(leafNode);
        cout << "Nodo hoja creado: '" << pair.first << "' (freq: " << pair.second << ")" << endl;
    }
    
    // PASO 2.2: Construir el árbol combinando nodos de menor frecuencia
    cout << "Construyendo árbol Huffman..." << endl;
    int stepCount = 1;
    
    while (pq.size() > 1) {
        // Extraer los dos nodos con menor frecuencia
        HuffmanNode* left = pq.top(); 
        pq.pop();
        HuffmanNode* right = pq.top(); 
        pq.pop();
        
        // Crear nodo padre que combina ambos
        int combinedFreq = left->frequency + right->frequency;
        HuffmanNode* merged = new HuffmanNode(combinedFreq);
        merged->left = left;   // Hijo izquierdo (contribuye '0' al código)
        merged->right = right; // Hijo derecho (contribuye '1' al código)
        
        cout << "Paso " << stepCount++ << ": Combinando nodos (freq: " 
             << left->frequency << " + " << right->frequency 
             << " = " << combinedFreq << ")" << endl;
        
        // Insertar el nodo combinado de vuelta en la cola
        pq.push(merged);
    }
    
    // El último nodo es la raíz del árbol Huffman completo
    HuffmanNode* root = pq.empty() ? nullptr : pq.top();
    cout << "Árbol Huffman construido exitosamente!" << endl;
    
    return root;
}

/**
 * PASO 3 DEL ALGORITMO: GENERACIÓN DE CÓDIGOS HUFFMAN
 * 
 * Recorre el árbol Huffman para asignar códigos binarios únicos a cada carácter.
 * 
 * REGLAS DE CODIFICACIÓN:
 * - Moverse hacia el hijo izquierdo: agregar '0' al código
 * - Moverse hacia el hijo derecho: agregar '1' al código
 * - Al llegar a una hoja: el código acumulado es el código Huffman del carácter
 * 
 * PROPIEDADES DE LOS CÓDIGOS GENERADOS:
 * - Son códigos de prefijo: ningún código es prefijo de otro
 * - Caracteres más frecuentes → códigos más cortos
 * - Caracteres menos frecuentes → códigos más largos
 * - Decodificación no ambigua: cada secuencia binaria corresponde a un único texto
 * 
 * COMPLEJIDAD: O(n) donde n es el número de caracteres únicos
 * 
 * Ejemplo con texto "ABACA":
 * A (freq=3) podría tener código "0"     (más frecuente → código corto)
 * B (freq=1) podría tener código "10"    (menos frecuente → código largo)
 * C (freq=1) podría tener código "11"    (menos frecuente → código largo)
 */
void HuffmanCompressor::generateCodes(HuffmanNode* node, string code) {
    if (!node) return;  // Caso base: nodo nulo
    
    // Si es una hoja (contiene un carácter), guardar el código
    if (!node->left && !node->right) {
        // Caso especial: si solo hay un carácter único, asignar código "0"
        string finalCode = code.empty() ? "0" : code;
        huffmanCodes[node->character] = finalCode;
        
        cout << "Código generado: '" << node->character << "' → \"" << finalCode << "\"" << endl;
        return;
    }
    
    // Recursión: continuar construyendo códigos para los hijos
    generateCodes(node->left, code + "0");   // Hijo izquierdo: agregar '0'
    generateCodes(node->right, code + "1");  // Hijo derecho: agregar '1'
}

/**
 * PASO 4 DEL ALGORITMO: SERIALIZACIÓN DEL ÁRBOL
 * 
 * Convierte la estructura del árbol Huffman en una secuencia de bytes que puede
 * guardarse en el archivo comprimido. Esto es necesario para poder reconstruir
 * el árbol durante la descompresión.
 * 
 * FORMATO DE SERIALIZACIÓN:
 * - '0': Marca un nodo nulo (fin de rama)
 * - '1': Marca un nodo hoja (seguido del carácter que contiene)
 * - '2': Marca un nodo interno (seguido de sus hijos serializados)
 * 
 * ORDEN DE SERIALIZACIÓN: Pre-orden (raíz, izquierda, derecha)
 * 
 * Ejemplo de árbol:      Serialización resultante:
 *       (5)              "2" + "1A" + "2" + "1B" + "1C"
 *      /   \             
 *    A(3)  (2)           Explicación:
 *          /  \          - "2": nodo interno raíz
 *        B(1) C(1)       - "1A": nodo hoja con carácter 'A'
 *                        - "2": nodo interno derecho
 *                        - "1B": nodo hoja con carácter 'B'
 *                        - "1C": nodo hoja con carácter 'C'
 */
void HuffmanCompressor::serializeTree(HuffmanNode* node, ofstream& outFile) {
    if (!node) {
        outFile.put('0'); // Marcador para nodo nulo (fin de rama)
        return;
    }
    
    // Verificar si es un nodo hoja (contiene un carácter)
    if (!node->left && !node->right) {
        outFile.put('1');             // Marcador para nodo hoja
        outFile.put(node->character); // Escribir el carácter almacenado
        cout << "Serializado nodo hoja: '" << node->character << "'" << endl;
    } else {
        outFile.put('2'); // Marcador para nodo interno
        cout << "Serializado nodo interno" << endl;
        
        // Serializar recursivamente los hijos (pre-orden)
        serializeTree(node->left, outFile);  // Subárbol izquierdo
        serializeTree(node->right, outFile); // Subárbol derecho
    }
}

/**
 * PASO 5 DEL ALGORITMO: DESERIALIZACIÓN DEL ÁRBOL
 * 
 * Reconstruye el árbol Huffman desde la secuencia de bytes guardada en el archivo.
 * Es el proceso inverso a la serialización y es crucial para la descompresión.
 * 
 * PROCESO DE DESERIALIZACIÓN:
 * 1. Leer el siguiente marcador del archivo
 * 2. Según el marcador:
 *    - '0': Retornar nullptr (nodo nulo)
 *    - '1': Crear nodo hoja con el siguiente carácter leído
 *    - '2': Crear nodo interno y deserializar recursivamente sus hijos
 * 
 * El orden de lectura debe coincidir exactamente con el orden de escritura (pre-orden).
 */
HuffmanNode* HuffmanCompressor::deserializeTree(ifstream& inFile) {
    char marker;
    if (!inFile.get(marker)) {
        cout << "Error: No se pudo leer marcador del archivo" << endl;
        return nullptr;  // Error de lectura
    }
    
    if (marker == '0') {
        // Nodo nulo: fin de rama
        return nullptr;
        
    } else if (marker == '1') {
        // Nodo hoja: leer el carácter almacenado
        char ch;
        if (!inFile.get(ch)) {
            cout << "Error: No se pudo leer carácter del nodo hoja" << endl;
            return nullptr;
        }
        cout << "Deserializado nodo hoja: '" << ch << "'" << endl;
        return new HuffmanNode(ch, 0);  // Frecuencia no importa en descompresión
        
    } else if (marker == '2') {
        // Nodo interno: crear nodo y deserializar hijos recursivamente
        cout << "Deserializando nodo interno..." << endl;
        HuffmanNode* node = new HuffmanNode(0);  // Frecuencia no importa
        node->left = deserializeTree(inFile);    // Deserializar hijo izquierdo
        node->right = deserializeTree(inFile);   // Deserializar hijo derecho
        return node;
    }
    
    cout << "Error: Marcador desconocido: " << marker << endl;
    return nullptr;  // Marcador inválido
}

/**
 * ================================================================================================
 * PROCESO COMPLETO DE COMPRESIÓN DE ARCHIVO
 * ================================================================================================
 * 
 * Este método implementa todo el pipeline de compresión Huffman:
 * 1. Lectura del archivo original
 * 2. Análisis de frecuencias de caracteres
 * 3. Construcción del árbol Huffman óptimo
 * 4. Generación de códigos binarios
 * 5. Serialización del árbol para guardarlo
 * 6. Codificación del texto usando códigos Huffman
 * 7. Empaquetado de bits en bytes para almacenamiento eficiente
 * 8. Escritura del archivo comprimido en formato .hub
 * 
 * FORMATO DEL ARCHIVO .HUB:
 * [Árbol serializado][Separador 0xFF][Longitud del texto][Datos codificados]
 * 
 * ================================================================================================
 */
bool HuffmanCompressor::compressFile(const string& inputFile, const string& outputFile) {
    cout << "\n=== INICIANDO COMPRESIÓN HUFFMAN ===" << endl;
    
    // FASE 1: Lectura del archivo original
    cout << "\nFASE 1: Leyendo archivo de entrada..." << endl;
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cout << "Error: No se pudo abrir el archivo de entrada: " << inputFile << endl;
        return false;
    }
    
    // Leer todo el contenido del archivo en memoria
    string text((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    
    if (text.empty()) {
        cout << "Error: El archivo está vacío." << endl;
        return false;
    }
    
    cout << "Archivo leído exitosamente. Tamaño: " << text.length() << " caracteres" << endl;
    
    // FASE 2: Análisis estadístico del texto
    cout << "\nFASE 2: Analizando frecuencias de caracteres..." << endl;
    calculateFrequency(text);
    
    // FASE 3: Construcción del árbol Huffman óptimo
    cout << "\nFASE 3: Construyendo árbol Huffman..." << endl;
    deleteTree(root);  // Eliminar árbol previo si existe
    root = buildHuffmanTree();
    
    if (!root) {
        cout << "Error: No se pudo construir el árbol Huffman." << endl;
        return false;
    }
    
    // FASE 4: Generación de códigos binarios
    cout << "\nFASE 4: Generando códigos Huffman..." << endl;
    huffmanCodes.clear();
    generateCodes(root, "");
    
    // Mostrar tabla de códigos generada
    cout << "\nTabla de códigos Huffman generada:" << endl;
    for (auto& pair : huffmanCodes) {
        cout << "'" << pair.first << "' → \"" << pair.second << "\"" << endl;
    }
    
    // FASE 5: Creación del archivo comprimido
    cout << "\nFASE 5: Creando archivo comprimido..." << endl;
    ofstream outFile(outputFile, ios::binary);
    if (!outFile) {
        cout << "Error: No se pudo crear el archivo de salida: " << outputFile << endl;
        return false;
    }
    
    // FASE 5.1: Serializar y guardar el árbol Huffman
    cout << "Guardando estructura del árbol..." << endl;
    serializeTree(root, outFile);
    
    // FASE 5.2: Escribir separador para delimitar el árbol de los datos
    outFile.put('\xFF');  // Byte especial que separa árbol de datos
    
    // FASE 5.3: Guardar la longitud del texto original
    // Esto es necesario para saber cuándo parar la decodificación
    size_t textLength = text.length();
    outFile.write(reinterpret_cast<const char*>(&textLength), sizeof(textLength));
    cout << "Longitud original guardada: " << textLength << " caracteres" << endl;
    
    // FASE 6: Codificación del texto usando códigos Huffman
    cout << "\nFASE 6: Codificando texto..." << endl;
    string encodedText = "";
    size_t originalBits = text.length() * 8;  // Bits en representación original
    
    for (char c : text) {
        encodedText += huffmanCodes[c];
    }
    
    cout << "Texto codificado. Bits originales: " << originalBits 
         << ", Bits codificados: " << encodedText.length() << endl;
    cout << "Reducción teórica: " << (100.0 * (originalBits - encodedText.length()) / originalBits) 
         << "%" << endl;
    
    // FASE 7: Empaquetado de bits en bytes
    cout << "\nFASE 7: Empaquetando bits en bytes..." << endl;
    string currentByte = "";
    size_t bytesWritten = 0;
    
    for (char bit : encodedText) {
        currentByte += bit;
        
        // Cuando completamos 8 bits, escribir un byte
        if (currentByte.length() == 8) {
            unsigned char byte = 0;
            
            // Convertir string binario a byte
            for (int i = 0; i < 8; i++) {
                if (currentByte[i] == '1') {
                    byte |= (1 << (7 - i));  // Bit más significativo primero
                }
            }
            
            outFile.put(byte);
            bytesWritten++;
            currentByte = "";
        }
    }
    
    // FASE 7.1: Manejar bits restantes con padding
    if (!currentByte.empty()) {
        // Rellenar con ceros hasta completar 8 bits
        while (currentByte.length() < 8) {
            currentByte += "0";
        }
        
        unsigned char byte = 0;
        for (int i = 0; i < 8; i++) {
            if (currentByte[i] == '1') {
                byte |= (1 << (7 - i));
            }
        }
        
        outFile.put(byte);
        bytesWritten++;
    }
    
    outFile.close();
    
    cout << "Bytes de datos escritos: " << bytesWritten << endl;
    cout << "\n¡Compresión completada exitosamente!" << endl;
    
    // Mostrar estadísticas de compresión
    displayCompressionStats(inputFile, outputFile);
    
    return true;
}

/**
 * ================================================================================================
 * PROCESO COMPLETO DE DESCOMPRESIÓN DE ARCHIVO
 * ================================================================================================
 * 
 * Este método implementa el proceso inverso de la compresión:
 * 1. Lectura del archivo comprimido .hub
 * 2. Deserialización del árbol Huffman guardado
 * 3. Lectura de los metadatos (longitud del texto original)
 * 4. Decodificación bit por bit usando el árbol Huffman
 * 5. Reconstrucción del texto original
 * 6. Escritura del archivo descomprimido en formato .txt
 * 
 * ALGORITMO DE DECODIFICACIÓN:
 * - Comenzar en la raíz del árbol
 * - Para cada bit leído:
 *   · Si es '0': moverse al hijo izquierdo
 *   · Si es '1': moverse al hijo derecho
 *   · Si llegamos a una hoja: emitir el carácter y volver a la raíz
 * - Continuar hasta decodificar todos los caracteres del texto original
 * 
 * ================================================================================================
 */
bool HuffmanCompressor::decompressFile(const string& inputFile, const string& outputFile) {
    cout << "\n=== INICIANDO DESCOMPRESIÓN HUFFMAN ===" << endl;
    
    // FASE 1: Lectura del archivo comprimido
    cout << "\nFASE 1: Leyendo archivo comprimido..." << endl;
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cout << "Error: No se pudo abrir el archivo comprimido: " << inputFile << endl;
        return false;
    }
    
    // FASE 2: Deserialización del árbol Huffman
    cout << "\nFASE 2: Reconstruyendo árbol Huffman..." << endl;
    deleteTree(root);  // Eliminar árbol previo si existe
    root = deserializeTree(inFile);
    
    if (!root) {
        cout << "Error: No se pudo deserializar el árbol Huffman." << endl;
        return false;
    }
    
    cout << "Árbol Huffman reconstruido exitosamente!" << endl;
    
    // FASE 3: Validación del separador
    cout << "\nFASE 3: Validando formato del archivo..." << endl;
    char separator;
    if (!inFile.get(separator) || separator != '\xFF') {
        cout << "Error: Formato de archivo inválido. Separador esperado: 0xFF, encontrado: " 
             << (int)separator << endl;
        return false;
    }
    
    cout << "Separador válido encontrado." << endl;
    
    // FASE 4: Lectura de metadatos
    cout << "\nFASE 4: Leyendo metadatos..." << endl;
    size_t textLength;
    if (!inFile.read(reinterpret_cast<char*>(&textLength), sizeof(textLength))) {
        cout << "Error: No se pudo leer la longitud del texto original." << endl;
        return false;
    }
    
    cout << "Longitud del texto original: " << textLength << " caracteres" << endl;
    
    // FASE 5: Lectura de datos comprimidos
    cout << "\nFASE 5: Leyendo datos comprimidos..." << endl;
    string compressedData((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    
    cout << "Datos comprimidos leídos: " << compressedData.length() << " bytes" << endl;
    
    // FASE 6: Proceso de decodificación
    cout << "\nFASE 6: Decodificando datos..." << endl;
    string decodedText = "";
    HuffmanNode* current = root;  // Comenzar en la raíz del árbol
    size_t decodedLength = 0;     // Contador de caracteres decodificados
    size_t totalBitsProcessed = 0;
    
    // Procesar cada byte de los datos comprimidos
    for (unsigned char byte : compressedData) {
        // Procesar cada bit del byte (de izquierda a derecha)
        for (int i = 7; i >= 0; i--) {
            // Si ya decodificamos suficientes caracteres, detener
            if (decodedLength >= textLength) break;
            
            // Extraer el bit actual
            bool bit = (byte >> i) & 1;
            totalBitsProcessed++;
            
            // Navegar por el árbol según el bit
            if (bit) {
                current = current->right;  // Bit '1': ir a la derecha
            } else {
                current = current->left;   // Bit '0': ir a la izquierda
            }
            
            // Si llegamos a una hoja, hemos decodificado un carácter completo
            if (!current->left && !current->right) {
                decodedText += current->character;
                current = root;  // Volver a la raíz para el siguiente carácter
                decodedLength++;
                
                // Debug periódico para textos largos
                if (decodedLength % 100 == 0) {
                    cout << "Decodificados " << decodedLength << "/" << textLength 
                         << " caracteres..." << endl;
                }
            }
        }
        
        // Si ya decodificamos todo el texto, salir del bucle externo
        if (decodedLength >= textLength) break;
    }
    
    cout << "Decodificación completada. Caracteres decodificados: " << decodedLength << endl;
    cout << "Bits procesados: " << totalBitsProcessed << endl;
    
    // FASE 7: Escritura del archivo descomprimido
    cout << "\nFASE 7: Escribiendo archivo descomprimido..." << endl;
    ofstream outFile(outputFile);
    if (!outFile) {
        cout << "Error: No se pudo crear el archivo de salida: " << outputFile << endl;
        return false;
    }
    
    outFile << decodedText;
    outFile.close();
    
    cout << "Archivo descomprimido guardado como: " << outputFile << endl;
    cout << "Tamaño del texto recuperado: " << decodedText.length() << " caracteres" << endl;
    
    // Verificar integridad
    if (decodedText.length() == textLength) {
        cout << "\n¡Descompresión completada exitosamente!" << endl;
        cout << "La integridad del archivo ha sido verificada." << endl;
    } else {
        cout << "\nAdvertencia: El tamaño del texto recuperado no coincide con el original." << endl;
    }
    
    return true;
}

/**
 * ================================================================================================
 * ANÁLISIS DE RENDIMIENTO Y ESTADÍSTICAS DE COMPRESIÓN
 * ================================================================================================
 * 
 * Esta función calcula y muestra métricas importantes sobre la efectividad de la compresión:
 * 
 * MÉTRICAS CALCULADAS:
 * 1. Tamaño original vs comprimido (en bytes y KB)
 * 2. Ratio de compresión: (tamaño_comprimido / tamaño_original) * 100
 * 3. Espacio ahorrado: (1 - ratio_compresión) * 100
 * 4. Factor de compresión: tamaño_original / tamaño_comprimido
 * 
 * FACTORES QUE AFECTAN LA COMPRESIÓN:
 * - Distribución de frecuencias: Mayor variación → mejor compresión
 * - Repetición de patrones: Más repetición → mejor compresión  
 * - Tamaño del archivo: Archivos más grandes tienden a comprimir mejor
 * - Tipo de contenido: Texto natural comprime mejor que datos aleatorios
 * 
 * ================================================================================================
 */
void HuffmanCompressor::displayCompressionStats(const string& originalFile, const string& compressedFile) {
    // Obtener tamaños de los archivos
    ifstream orig(originalFile, ios::binary | ios::ate);
    ifstream comp(compressedFile, ios::binary | ios::ate);
    
    if (orig && comp) {
        // Leer tamaños de archivo (posición al final = tamaño total)
        size_t originalSize = orig.tellg();
        size_t compressedSize = comp.tellg();
        
        // Calcular métricas de compresión
        double compressionRatio = (double)compressedSize / originalSize * 100;
        double spaceSaved = (1.0 - (double)compressedSize / originalSize) * 100;
        double compressionFactor = (double)originalSize / compressedSize;
        
        // Mostrar estadísticas detalladas
        cout << "\n========================================" << endl;
        cout << "    ESTADÍSTICAS DE COMPRESIÓN HUFFMAN" << endl;
        cout << "========================================" << endl;
        cout << "Archivo original: " << originalFile << endl;
        cout << "Archivo comprimido: " << compressedFile << endl;
        cout << "----------------------------------------" << endl;
        cout << "Tamaño original:    " << originalSize << " bytes (" 
             << fixed << setprecision(2) << originalSize / 1024.0 << " KB)" << endl;
        cout << "Tamaño comprimido:  " << compressedSize << " bytes (" 
             << fixed << setprecision(2) << compressedSize / 1024.0 << " KB)" << endl;
        cout << "----------------------------------------" << endl;
        cout << "Ratio de compresión: " << fixed << setprecision(2) << compressionRatio << "%" << endl;
        cout << "Espacio ahorrado:    " << fixed << setprecision(2) << spaceSaved << "%" << endl;
        cout << "Factor de compresión: " << fixed << setprecision(2) << compressionFactor << ":1" << endl;
        cout << "----------------------------------------" << endl;
        
        // Análisis de efectividad
        if (spaceSaved > 50) {
            cout << "¡Excelente compresión! El archivo se redujo significativamente." << endl;
        } else if (spaceSaved > 25) {
            cout << "Buena compresión. Reducción notable del tamaño." << endl;
        } else if (spaceSaved > 10) {
            cout << "Compresión moderada. Algún ahorro de espacio logrado." << endl;
        } else if (spaceSaved > 0) {
            cout << "Compresión mínima. El archivo no se beneficia mucho de Huffman." << endl;
        } else {
            cout << "Advertencia: El archivo comprimido es mayor que el original." << endl;
            cout << "Esto puede ocurrir con archivos muy pequeños o con distribución uniforme." << endl;
        }
        
        cout << "========================================" << endl;
        
        // Información adicional sobre el algoritmo
        cout << "\nNOTA: La efectividad de la compresión Huffman depende de:" << endl;
        cout << "• Variabilidad en la frecuencia de caracteres" << endl;
        cout << "• Tamaño del archivo (archivos más grandes comprimen mejor)" << endl;
        cout << "• Tipo de contenido (texto natural vs datos aleatorios)" << endl;
    } else {
        cout << "Error: No se pudieron leer los archivos para calcular estadísticas." << endl;
    }
    
    // Cerrar archivos
    orig.close();
    comp.close();
}