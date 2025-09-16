# Herramienta de Compresión Huffman (.HUB)

Esta es una implementación sencilla de compresión y descompresión usando el algoritmo de Huffman.

## Características
- Menú interactivo en consola.
- Compresión de archivos a formato `.HUB`.
- Descompresión de archivos `.HUB` a `.txt` (o al nombre original si lo proporcionas manualmente).
- Formato incluye: magia `HUB1`, tamaño original, número de símbolos, tabla de frecuencias y flujo de bits codificados.

## Compilación
Requisitos mínimos: Compilador C++17 (g++, clang o MSVC) y CMake (opcional pero recomendado para Windows).

### Opción A (Linux / WSL / macOS) con Makefile
```bash
make
```
Genera el binario `huffman_tool`.

Limpiar:
```bash
make clean
```

### Opción B (Multiplataforma) con CMake
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
Resultado: `build/huffman_tool` (en Windows: `build/Release/huffman_tool.exe` si usa generador multi-config).

#### Ejemplos específicos
Linux / macOS (Ninja):
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Windows (PowerShell, MSVC - Visual Studio Build Tools):
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
Binario: `build/Release/huffman_tool.exe`

Windows (MinGW):
```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
Binario: `build/huffman_tool.exe`

### Notas de portabilidad
- Se evita cualquier llamada POSIX específica: sólo C++ estándar.
- El formato escribe explícitamente en little-endian, garantizando consistencia entre arquitecturas.
- `std::filesystem` requiere C++17; en compiladores muy antiguos de GCC (<9) podría necesitar `-lstdc++fs`, CMake lo intenta detectar.
- Para rutas con espacios (Windows / Linux) escribirlas entre comillas al introducirlas en el menú.

## Uso
Ejecuta:
```bash
./huffman_tool
```
Sigue el menú:
1. Comprimir: Ingresa la ruta del archivo a comprimir. Crea `archivo.original.HUB`.
2. Descomprimir: Ingresa la ruta del `.HUB`. Crea `archivo.HUB.txt`.
3. Salir.

## Notas sobre la compresión
La tasa de compresión depende del contenido. Archivos con mucha repetición (texto, logs, etc.) comprimen mejor. Archivos ya comprimidos (imágenes JPEG, ZIP, MP4) usualmente no mejorarán e incluso pueden crecer ligeramente debido a la cabecera.

## Estructura del Formato `.HUB`
```
Offset  Tamaño  Descripción
0       4       Magic 'HUB1'
4       8       Tamaño original (uint64 little-endian)
12      2       Número de símbolos distintos (uint16 little-endian)
14      N*(1+8) Tabla: por cada símbolo -> 1 byte valor + 8 bytes frecuencia
...             Datos codificados en bits (MSB-first por byte)
EOF-8   8       Número total de bits útiles en el flujo codificado
```

## Posibles Mejoras Futuras
- Guardar nombre original del archivo y extensión.
- Detección de falta de compresión útil (si no reduce, avisar).
- Multihilo para construir frecuencias en archivos grandes.
- Añadir pruebas unitarias.
- Opción para especificar archivo de salida manualmente en el menú.
- Verificación de integridad (checksum) en el encabezado.

## Licencia
Uso libre educativo.
