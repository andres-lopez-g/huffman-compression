# 🗜️ Huffman Compression Tool

Un programa simple y eficiente de compresión usando el algoritmo de Huffman desarrollado en C++17.

## 📋 Características

- **Compresión Huffman**: Implementación completa del algoritmo de codificación Huffman
- **Interfaz simple**: Menú interactivo fácil de usar
- **Archivos .HUB**: Formato personalizado para archivos comprimidos
- **Estadísticas**: Muestra ratios de compresión y tamaños de archivo
- **Multiplataforma**: Compatible con Windows, Linux y macOS

## 🚀 Compilación

### Prerrequisitos
- Compilador C++17 compatible (GCC, Clang, MSVC)
- Make (opcional)

### Opción 1: Usando Makefile (Recomendado)
```bash
# Compilar
make

# Ejecutar
make run

# Limpiar archivos compilados
make clean
```

### Opción 2: Compilación manual
```bash
g++ -std=c++17 -Wall -Wextra -O2 src/main.cpp src/huffman.cpp -o huffman_tool.exe
```

### Opción 3: Usando CMake (si tienes compilador instalado)
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## 📖 Uso

1. **Ejecutar el programa**:
   ```bash
   ./huffman_tool.exe
   ```

2. **Comprimir un archivo**:
   - Selecciona la opción `1` del menú
   - Ingresa la ruta del archivo a comprimir
   - El archivo comprimido se guardará con extensión `.HUB`

3. **Descomprimir un archivo**:
   - Selecciona la opción `2` del menú
   - Ingresa la ruta del archivo `.HUB` a descomprimir
   - El archivo se descomprimirá con extensión `.txt`

## 📁 Estructura del Proyecto

```
huffman-compression/
├── src/
│   ├── main.cpp          # Programa principal con menú interactivo
│   ├── huffman.cpp       # Implementación del algoritmo Huffman
│   └── huffman.hpp       # Declaraciones de la clase HuffmanCompressor
├── CMakeLists.txt        # Configuración CMake (opcional)
├── Makefile             # Makefile simplificado
└── README.md            # Este archivo
```

## 🔧 Funcionalidades

### Compresión
- Lee archivos de cualquier tipo
- Construye tabla de frecuencias de bytes
- Genera árbol de Huffman optimizado
- Codifica datos usando códigos de longitud variable
- Guarda resultado en formato `.HUB` personalizado

### Descompresión
- Lee archivos `.HUB`
- Reconstruye árbol de Huffman desde tabla de frecuencias
- Decodifica datos bit por bit
- Restaura archivo original

## 📊 Formato de Archivo .HUB

El formato .HUB incluye:
1. **Magic number**: "HUB1" (4 bytes)
2. **Tamaño original**: Bytes del archivo original (8 bytes)
3. **Número de símbolos**: Cantidad de bytes únicos (2 bytes)
4. **Tabla de frecuencias**: Pares (byte, frecuencia) para cada símbolo
5. **Datos codificados**: Bits codificados con Huffman
6. **Total de bits**: Número exacto de bits codificados (8 bytes)

## 💡 Consejos de Uso

- **Archivos de texto** comprimen mejor (pueden reducirse 40-60%)
- **Archivos ya comprimidos** (ZIP, JPG, MP3) pueden crecer ligeramente
- **Usa comillas** si la ruta contiene espacios: `"C:\Mi Carpeta\archivo.txt"`
- **Archivos pequeños** pueden no comprimir bien debido a overhead

## 🎯 Ejemplos

### Compresión de un archivo de texto:
```
📁 Ingrese la ruta del archivo a comprimir: documento.txt

🗜️  Iniciando compresión...
📊 Tamaño original: 1024 bytes
✅ Compresión completada exitosamente!
📦 Archivo comprimido: 612 bytes
📈 Ratio de compresión: 40.23%
💾 Guardado como: documento.txt.HUB
```

### Descompresión:
```
📁 Ingrese la ruta del archivo .HUB a descomprimir: documento.txt.HUB

📤 Iniciando descompresión...
📊 Tamaño original: 1024 bytes
✅ Descompresión completada exitosamente!
📦 Bytes descomprimidos: 1024
💾 Guardado como: documento.txt.HUB.txt
```

## 🔬 Algoritmo de Huffman

Este programa implementa el algoritmo clásico de codificación Huffman:

1. **Análisis de frecuencias**: Cuenta la frecuencia de cada byte
2. **Construcción del árbol**: Crea árbol binario usando cola de prioridad
3. **Generación de códigos**: Asigna códigos binarios de longitud variable
4. **Codificación**: Reemplaza bytes originales con códigos Huffman
5. **Decodificación**: Recorre el árbol para restaurar datos originales

## 🏗️ Compiladores Soportados

- **GCC 7+** (Linux, Windows con MinGW)
- **Clang 6+** (macOS, Linux)
- **MSVC 2017+** (Windows con Visual Studio)

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Para cambios importantes:

1. Fork el proyecto
2. Crea una rama para tu feature
3. Commit tus cambios
4. Push a la rama
5. Abre un Pull Request

## 📄 Licencia

Este proyecto está bajo licencia MIT. Ver archivo LICENSE para más detalles.

## 👨‍💻 Autor

Desarrollado como proyecto académico para demostrar la implementación del algoritmo de compresión Huffman.