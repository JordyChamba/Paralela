cmake_minimum_required(VERSION 3.16)
project(FractalJulia)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -fopenmp")

# Toolchain de vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/tools/vcpkg-2026.03.18/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic")

# Ejecutable
add_executable(
    fractal_julia 
    main.cpp 
    palette.cpp
    fractal_serial.cpp
    fractal_simd.cpp
    fractal_openmp.cpp
)

# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(fractal_julia PRIVATE fmt::fmt)

# OpenCV
# find_package(OpenCV CONFIG REQUIRED)
# target_link_libraries(fractal_julia PRIVATE ${OpenCV_LIBS})
# target_link_libraries(fractal_julia PRIVATE SFML::Main)

# SFML (sin fijar versión)
find_package(SFML COMPONENTS System Window Graphics Network Audio REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)
target_link_libraries(fractal_julia PRIVATE
    SFML::Network
    SFML::Graphics
    SFML::Window
    SFML::Audio
    SFML::System
)

