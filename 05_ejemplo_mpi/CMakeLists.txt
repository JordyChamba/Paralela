cmake_minimum_required(VERSION 3.16)

project(ejemploMPI)

find_package(fmt CONFIG REQUIRED)

#config MPI
set(MPI_ROOT "C:/oneAPI/mpi/latest")
set(MPI_INCLUDE "${MPI_ROOT}/include")

# Libreria para enlazar
set(MPI_LIB "${MPI_ROOT}/lib")

include_directories(${MPI_INCLUDE}) # Aqui incluye archivos .h

add_executable(ejemplo_mpi main.cpp matrices.cpp)

target_link_libraries(ejemplo_mpi PRIVATE fmt::fmt ${MPI_LIB}/impi.lib )