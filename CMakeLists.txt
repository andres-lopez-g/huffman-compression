cmake_minimum_required(VERSION 3.12)
project(huffman_tool LANGUAGES CXX VERSION 1.0.0)

# Configuración estándar
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opción para compilar como librería (futuro)
option(HUFFMAN_BUILD_LIB "Build Huffman library" OFF)

# Archivos fuente
set(HUFFMAN_SOURCES
    src/huffman.cpp
    src/main.cpp
)

add_executable(huffman_tool ${HUFFMAN_SOURCES})

# Incluir directorio de cabeceras
target_include_directories(huffman_tool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Flags específicos según compilador
if(MSVC)
    target_compile_options(huffman_tool PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(huffman_tool PRIVATE -Wall -Wextra -Wpedantic -O2)
endif()

# Algunas implementaciones antiguas de libstdc++ (GCC<9) requieren enlazar stdc++fs
# Intento condicional: sólo si la característica no se detecta
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("#include <filesystem>\nint main(){std::filesystem::path p{"."}; return 0;}" HAVE_STD_FILESYSTEM)
if(NOT HAVE_STD_FILESYSTEM)
    # Intentar forzar enlace explícito
    find_library(STDCXXFS_LIB stdc++fs)
    if(STDCXXFS_LIB)
        target_link_libraries(huffman_tool PRIVATE ${STDCXXFS_LIB})
    endif()
endif()

# Agrupar en IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HUFFMAN_SOURCES})

# Instalación opcional
install(TARGETS huffman_tool RUNTIME DESTINATION bin)
