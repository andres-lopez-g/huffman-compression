# Huffman Compression Tool

Un programa simple y eficiente de compresion usando el algoritmo de Huffman desarrollado en C++17.

## Caracteristicas

- **Compresion Huffman**: Implementacion completa del algoritmo de codificacion Huffman
- **Interfaz simple**: Menu interactivo facil de usar
- **Archivos .HUB**: Formato personalizado para archivos comprimidos
- **Estadisticas**: Muestra ratios de compresion y tamanios de archivo
- **Multiplataforma**: Compatible con Windows, Linux y macOS

## Compilacion

### Prerrequisitos
- Compilador C++17 compatible (GCC, Clang, MSVC)
- Make (opcional)

### Opcion 1: Usando Makefile (Recomendado)
```bash
# Compilar
make

# Ejecutar
make run

# Limpiar archivos compilados
make clean
```

### Opcion 2: Compilacion manual
```bash
g++ -std=c++17 -Wall -Wextra -O2 src/main.cpp src/huffman.cpp -o huffman_tool.exe
```

### Opcion 3: Usando CMake (si tienes compilador instalado)
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Uso

1. **Ejecutar el programa**:
   ```bash
   ./huffman_tool.exe
   ```

2. **Comprimir un archivo**:
   - Selecciona la opcion `1` del menu
   - Ingresa la ruta del archivo a comprimir
   - Opcionalmente especifica un nombre personalizado para el archivo comprimido
   - El archivo comprimido se guardara con extension `.HUB`

3. **Descomprimir un archivo**:
   - Selecciona la opcion `2` del menu
   - Ingresa la ruta del archivo `.HUB` a descomprimir
   - Opcionalmente especifica un nombre personalizado para el archivo descomprimido
   - El archivo se descomprimira con la extension original

## Estructura del Proyecto

```
huffman-compression/
├── src/
│   ├── main.cpp          # Programa principal con menu interactivo
│   ├── huffman.cpp       # Implementacion del algoritmo Huffman
│   └── huffman.hpp       # Declaraciones de la clase HuffmanCompressor
├── CMakeLists.txt        # Configuracion CMake (opcional)
├── Makefile             # Makefile simplificado
└── README.md            # Este archivo
```

## Funcionalidades

### Compresion
- Lee archivos de cualquier tipo
- Construye tabla de frecuencias de bytes
- Genera arbol de Huffman optimizado
- Codifica datos usando codigos de longitud variable
- Guarda resultado en formato `.HUB` personalizado
- Permite especificar nombre personalizado para archivo comprimido

### Descompresion
- Lee archivos `.HUB`
- Reconstruye arbol de Huffman desde tabla de frecuencias
- Decodifica datos bit por bit
- Restaura archivo original
- Permite especificar nombre personalizado para archivo descomprimido

## Formato de Archivo .HUB

El formato .HUB incluye:
1. **Magic number**: "HUB1" (4 bytes)
2. **Tamanio original**: Bytes del archivo original (8 bytes)
3. **Numero de simbolos**: Cantidad de bytes unicos (2 bytes)
4. **Tabla de frecuencias**: Pares (byte, frecuencia) para cada simbolo
5. **Datos codificados**: Bits codificados con Huffman
6. **Total de bits**: Numero exacto de bits codificados (8 bytes)

## Consejos de Uso

- **Archivos de texto** comprimen mejor (pueden reducirse 40-60%)
- **Archivos ya comprimidos** (ZIP, JPG, MP3) pueden crecer ligeramente
- **Usa comillas** si la ruta contiene espacios: `"C:\Mi Carpeta\archivo.txt"`
- **Archivos pequenios** pueden no comprimir bien debido a overhead

## Ejemplos

### Compresion de un archivo de texto:
```
Ingrese la ruta del archivo a comprimir: documento.txt
Desea especificar un nombre para el archivo comprimido? (s/n): s
Ingrese el nombre del archivo de salida (sin extension): mi_archivo

Iniciando compresion...
Tamanio original: 1024 bytes
Compresion completada exitosamente!
Archivo comprimido: 612 bytes
Ratio de compresion: 40.23%
Guardado como: mi_archivo.HUB
```

### Descompresion:
```
Ingrese la ruta del archivo .HUB a descomprimir: mi_archivo.HUB
Desea especificar un nombre para el archivo descomprimido? (s/n): n

Iniciando descompresion...
Tamanio original: 1024 bytes
Descompresion completada exitosamente!
Bytes descomprimidos: 1024
Guardado como: mi_archivo.HUB.txt
```

## Algoritmo de Huffman

Este programa implementa el algoritmo clasico de codificacion Huffman:

1. **Analisis de frecuencias**: Cuenta la frecuencia de cada byte
2. **Construccion del arbol**: Crea arbol binario usando cola de prioridad
3. **Generacion de codigos**: Asigna codigos binarios de longitud variable
4. **Codificacion**: Reemplaza bytes originales con codigos Huffman
5. **Decodificacion**: Recorre el arbol para restaurar datos originales

## Compiladores Soportados

- **GCC 7+** (Linux, Windows con MinGW)
- **Clang 6+** (macOS, Linux)
- **MSVC 2017+** (Windows con Visual Studio)

## Contribuciones

Las contribuciones son bienvenidas. Para cambios importantes:

1. Fork el proyecto
2. Crea una rama para tu feature
3. Commit tus cambios
4. Push a la rama
5. Abre un Pull Request

## Licencia

Este proyecto esta bajo licencia MIT. Ver archivo LICENSE para mas detalles.

## Autor

Desarrollado como proyecto academico para demostrar la implementacion del algoritmo de compresion Huffman.