# Compresor Huffman en C++
**Autor: Andres J Lopez**

## Descripción
Este programa implementa el algoritmo de compresión Huffman en C++ con una interfaz de usuario completa que permite comprimir y descomprimir archivos de texto. El algoritmo utiliza códigos de longitud variable para lograr una compresión eficiente sin pérdida de información.

## Características Principales
- ✅ Compresión de archivos a formato `.hub` (Huffman)
- ✅ Descompresión de archivos `.hub` a `.txt` 
- ✅ Interfaz de menú intuitiva en español
- ✅ Estadísticas detalladas de compresión
- ✅ Reducción significativa del tamaño de archivo (típicamente 40-60%)
- ✅ Validación de integridad completa
- ✅ Documentación exhaustiva del algoritmo

## Metodología del Algoritmo Huffman

### Proceso de Compresión:
1. **Análisis de Frecuencias**: Cuenta la aparición de cada carácter
2. **Construcción del Árbol**: Crea un árbol binario óptimo usando cola de prioridad
3. **Generación de Códigos**: Asigna códigos binarios basados en frecuencias
4. **Codificación**: Reemplaza caracteres por códigos Huffman
5. **Serialización**: Guarda el árbol y datos en formato `.hub`

### Proceso de Descompresión:
1. **Deserialización**: Reconstruye el árbol desde el archivo
2. **Decodificación**: Navega el árbol bit por bit para recuperar caracteres
3. **Reconstrucción**: Regenera el texto original completo

## Compilación y Uso

### Compilar el programa:
```bash
g++ -o compresor_huffman.exe main.cpp huffman.cpp
```

### Ejecutar:
```bash
./compresor_huffman.exe
```

### Uso del programa:
1. **Ejecutar el programa**
2. **Seleccionar opción del menú**:
   - `1`: Comprimir archivo (genera archivo `.hub`)
   - `2`: Descomprimir archivo (genera archivo `.txt`)
   - `3`: Salir del programa
3. **Ingresar la ruta completa del archivo**
4. **El programa genera automáticamente el archivo de salida**

## Estructura del Proyecto
```
compresion huffman/
├── main.cpp                    # Programa principal con interfaz de usuario
├── huffman.h                   # Declaraciones de clases y estructuras
├── huffman.cpp                 # Implementación completa del algoritmo
├── documentacion_completa.h    # Documentación técnica exhaustiva
├── README.md                   # Este archivo
├── test.txt                    # Archivo de prueba para testing
└── compresor_huffman.exe       # Ejecutable compilado
```

## Ejemplos de Resultados

### Archivo de Prueba (test.txt - 554 bytes):
```
Tamaño original:    554 bytes (0.54 KB)
Tamaño comprimido:  287 bytes (0.28 KB)
Ratio de compresión: 51.81%
Espacio ahorrado:    48.19%
Factor de compresión: 1.93:1
```

## Documentación Técnica Completa

El proyecto incluye documentación exhaustiva que explica:

### Teoría del Algoritmo:
- Principios fundamentales de la codificación Huffman
- Análisis matemático de la compresión
- Ejemplos paso a paso con datos reales

### Implementación Detallada:
- Explicación línea por línea del código
- Estructuras de datos utilizadas
- Algoritmos de construcción del árbol
- Procesos de serialización/deserialización

### Análisis de Rendimiento:
- Complejidad temporal y espacial
- Factores que afectan la compresión
- Casos de uso recomendados y no recomendados

### Optimizaciones:
- Técnicas de empaquetado de bits
- Gestión eficiente de memoria
- Validaciones robustas de integridad

## Casos de Uso Ideales
- ✅ **Archivos de texto** (.txt, .csv, .log)
- ✅ **Código fuente** (.cpp, .h, .js, .py)
- ✅ **Documentos estructurados** (.xml, .html)
- ✅ **Archivos de configuración** (.ini, .conf)

## Limitaciones
- ❌ **Archivos ya comprimidos** (.zip, .rar, .7z)
- ❌ **Imágenes comprimidas** (.jpg, .png, .gif)
- ❌ **Audio/video** (.mp3, .mp4, .avi)
- ❌ **Archivos muy pequeños** (<1KB)

## Tecnologías Utilizadas
- **Lenguaje**: C++14 o superior
- **Compilador**: GCC/MinGW compatible
- **Estructuras de datos**: 
  - `std::priority_queue` para construcción del árbol
  - `std::unordered_map` para mapeo de códigos
  - Árboles binarios dinámicos
- **Algoritmos**: Huffman coding, serialización binaria

## Autor
**Andres J Lopez**  
Implementación completa del algoritmo de compresión Huffman con documentación técnica exhaustiva.