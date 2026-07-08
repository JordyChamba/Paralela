cmake_minimum_required(VERSION 3.16)

project(Cuda01)

# Configurar el estándar C++17 exigido por SFML 3
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CUDA_ARCHITECTURES 75)

enable_language(CXX CUDA)

find_package(fmt CONFIG REQUIRED)

# CORRECCIÓN SFML 3: Componentes ahora inician con Mayúscula (Graphics, Window, System)
find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window System)

add_executable(cuda01 main.cpp kernel.cu palette.cpp)

target_include_directories(cuda01 PRIVATE
    ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
)

# CORRECCIÓN SFML 3: Se enlazan usando el namespace oficial SFML::
target_link_libraries(cuda01 PRIVATE 
    fmt::fmt
    SFML::Graphics
    SFML::Window
    SFML::System
)