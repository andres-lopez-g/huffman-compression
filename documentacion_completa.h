/*
 * ================================================================================================
 * DOCUMENTACIÓN COMPLETA: COMPRESOR HUFFMAN EN C++
 * Autor: Andres J Lopez
 * ================================================================================================
 * 
 * RESUMEN DEL PROYECTO:
 * Este programa implementa el algoritmo de compresión Huffman, un método de compresión sin 
 * pérdida que utiliza códigos de longitud variable para lograr una reducción significativa 
 * del tamaño de archivos de texto.
 * 
 * ================================================================================================
 * TEORÍA DEL ALGORITMO HUFFMAN
 * ================================================================================================
 * 
 * PRINCIPIO FUNDAMENTAL:
 * El algoritmo de Huffman se basa en la idea de que los caracteres más frecuentes en un texto
 * deben representarse con códigos más cortos, mientras que los menos frecuentes pueden usar
 * códigos más largos. Esto minimiza la longitud promedio de codificación.
 * 
 * EJEMPLO CONCEPTUAL:
 * Texto: "ABACABA"
 * Frecuencias: A=4, B=2, C=1
 * 
 * Códigos ASCII (8 bits por carácter):
 * A=01000001, B=01000010, C=01000011
 * Total: 7 caracteres × 8 bits = 56 bits
 * 
 * Códigos Huffman optimizados:
 * A=0 (más frecuente, código más corto)
 * B=10 
 * C=11 (menos frecuente, código más largo)
 * 
 * Texto codificado: "0100110100" (10 bits vs 56 bits originales)
 * Reducción: 82% de compresión!
 * 
 * ================================================================================================
 * METODOLOGÍA DE IMPLEMENTACIÓN
 * ================================================================================================
 * 
 * FASE 1: ANÁLISIS ESTADÍSTICO
 * ───────────────────────────────
 * • Leer archivo completo en memoria
 * • Contar frecuencia de cada carácter único
 * • Crear tabla de frecuencias
 * 
 * FASE 2: CONSTRUCCIÓN DEL ÁRBOL HUFFMAN
 * ──────────────────────────────────────
 * • Crear nodos hoja para cada carácter con su frecuencia
 * • Usar cola de prioridad (min-heap) para ordenar por frecuencia
 * • Algoritmo de construcción:
 *   1. Extraer dos nodos con menor frecuencia
 *   2. Crear nodo padre con suma de frecuencias
 *   3. Asignar nodos extraídos como hijos izquierdo y derecho
 *   4. Insertar nodo padre en la cola
 *   5. Repetir hasta que quede un solo nodo (la raíz)
 * 
 * ESTRUCTURA DEL ÁRBOL RESULTANTE:
 *       [Raíz]
 *      /      \
 *   [Nodo]   [Hoja]
 *   /    \      |
 * [Hoja][Hoja] char
 * 
 * FASE 3: GENERACIÓN DE CÓDIGOS
 * ─────────────────────────────
 * • Recorrer árbol desde raíz hasta hojas
 * • Camino izquierdo: agregar '0' al código
 * • Camino derecho: agregar '1' al código
 * • En cada hoja: guardar código completo para ese carácter
 * 
 * FASE 4: CODIFICACIÓN DE DATOS
 * ─────────────────────────────
 * • Reemplazar cada carácter por su código Huffman
 * • Concatenar todos los códigos en una secuencia binaria
 * • Empaquetar bits en bytes para almacenamiento eficiente
 * 
 * FASE 5: SERIALIZACIÓN Y ALMACENAMIENTO
 * ──────────────────────────────────────
 * • Serializar estructura del árbol en formato binario
 * • Guardar metadatos (longitud del texto original)
 * • Escribir datos codificados
 * • Formato final del archivo .hub:
 *   [Árbol serializado][0xFF][Longitud][Datos comprimidos]
 * 
 * ================================================================================================
 * PROCESO DE DESCOMPRESIÓN
 * ================================================================================================
 * 
 * FASE 1: LECTURA Y VALIDACIÓN
 * ─────────────────────────────
 * • Leer archivo .hub comprimido
 * • Validar formato y integridad
 * 
 * FASE 2: RECONSTRUCCIÓN DEL ÁRBOL
 * ─────────────────────────────────
 * • Deserializar árbol desde formato binario
 * • Reconstruir estructura exacta del árbol original
 * 
 * FASE 3: DECODIFICACIÓN
 * ──────────────────────
 * • Procesar datos bit por bit
 * • Navegar por el árbol según cada bit:
 *   - Bit '0': ir al hijo izquierdo
 *   - Bit '1': ir al hijo derecho
 * • Al llegar a una hoja: emitir carácter y volver a la raíz
 * • Continuar hasta decodificar todos los caracteres
 * 
 * FASE 4: RECONSTRUCCIÓN DEL ARCHIVO
 * ──────────────────────────────────
 * • Escribir texto decodificado a archivo .txt
 * • Verificar integridad comparando longitudes
 * 
 * ================================================================================================
 * ANÁLISIS DE COMPLEJIDAD
 * ================================================================================================
 * 
 * COMPLEJIDAD TEMPORAL:
 * • Análisis de frecuencias: O(n) donde n = longitud del texto
 * • Construcción del árbol: O(k log k) donde k = caracteres únicos
 * • Generación de códigos: O(k)
 * • Codificación: O(n)
 * • Total compresión: O(n + k log k)
 * • Descompresión: O(n)
 * 
 * COMPLEJIDAD ESPACIAL:
 * • Árbol Huffman: O(k) nodos
 * • Tabla de códigos: O(k)
 * • Texto en memoria: O(n)
 * • Total: O(n + k)
 * 
 * ================================================================================================
 * FACTORES QUE AFECTAN LA COMPRESIÓN
 * ================================================================================================
 * 
 * FACTORES POSITIVOS (mejoran compresión):
 * • Alta variabilidad en frecuencias de caracteres
 * • Textos con muchas repeticiones
 * • Archivos de tamaño mediano a grande
 * • Contenido de texto natural (no aleatorio)
 * • Idiomas con distribución desigual de letras
 * 
 * FACTORES NEGATIVOS (reducen compresión):
 * • Distribución uniforme de caracteres
 * • Archivos muy pequeños (overhead del árbol)
 * • Datos completamente aleatorios
 * • Archivos ya comprimidos (.zip, .jpg, etc.)
 * 
 * ================================================================================================
 * OPTIMIZACIONES IMPLEMENTADAS
 * ================================================================================================
 * 
 * 1. SERIALIZACIÓN EFICIENTE:
 *    • Formato binario compacto para el árbol
 *    • Marcadores de 1 byte para identificar tipos de nodo
 * 
 * 2. EMPAQUETADO DE BITS:
 *    • Conversión de códigos binarios a bytes reales
 *    • Padding automático para completar bytes
 * 
 * 3. GESTIÓN DE MEMORIA:
 *    • Destrucción automática del árbol
 *    • Prevención de memory leaks
 * 
 * 4. VALIDACIONES ROBUSTAS:
 *    • Verificación de formato de archivo
 *    • Manejo de errores de lectura/escritura
 *    • Validación de integridad en descompresión
 * 
 * ================================================================================================
 * CASOS DE USO RECOMENDADOS
 * ================================================================================================
 * 
 * IDEAL PARA:
 * • Archivos de texto (.txt, .csv, .log)
 * • Código fuente (.cpp, .h, .js, .py)
 * • Documentos estructurados (.xml, .html)
 * • Archivos de configuración (.ini, .conf)
 * 
 * NO RECOMENDADO PARA:
 * • Archivos ya comprimidos (.zip, .rar, .7z)
 * • Imágenes comprimidas (.jpg, .png, .gif)
 * • Audio/video comprimidos (.mp3, .mp4, .avi)
 * • Archivos binarios aleatorios
 * • Archivos muy pequeños (<1KB)
 * 
 * ================================================================================================
 * EJEMPLO DE RESULTADOS ESPERADOS
 * ================================================================================================
 * 
 * ARCHIVO DE PRUEBA (554 bytes):
 * Texto en español con repeticiones de palabras comunes
 * 
 * RESULTADOS TÍPICOS:
 * • Compresión esperada: 40-60%
 * • Archivo original: 554 bytes
 * • Archivo comprimido: ~220-330 bytes
 * • Factores: distribución del español, repetición de palabras
 * 
 * CASOS EXTREMOS:
 * • Mejor caso: texto con mucha repetición (80%+ compresión)
 * • Peor caso: datos aleatorios (puede aumentar tamaño)
 * 
 * ================================================================================================
 */

#ifndef HUFFMAN_DOCUMENTATION_H
#define HUFFMAN_DOCUMENTATION_H

/*
 * Este archivo sirve como documentación completa del proyecto.
 * Para utilizar el compresor:
 * 
 * 1. Compilar: g++ -o huffman_compressor.exe main.cpp huffman.cpp
 * 2. Ejecutar: ./huffman_compressor.exe
 * 3. Seguir el menú interactivo
 * 
 * El programa creará archivos .hub para compresión y .txt para descompresión.
 */

#endif