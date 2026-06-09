# "C:\oneAPI\mpi\2021.18\bin\mpiexec.exe" -n 8 ejemplo_mpi.exe
# "C:\oneAPI\mpi\2021.18\bin\mpiexec.exe" -n 8 ejemplo_mpi_V2.exe


cmake_minimum_required(VERSION 3.16)

project(ejemploMPI)

find_package(fmt CONFIG REQUIRED)

#config MPI
set(MPI_ROOT "C:/oneAPI/mpi/2021.18")
set(MPI_INCLUDE "${MPI_ROOT}/include")

# Libreria para enlazar
set(MPI_LIB "${MPI_ROOT}/lib")

include_directories(${MPI_INCLUDE}) # Aqui incluye archivos .h

add_executable(ejemplo_mpi main.cpp matrices.cpp)

target_link_libraries(ejemplo_mpi PRIVATE fmt::fmt ${MPI_LIB}/impi.lib )

# Copiar impi.dll a la carpeta del ejecutable para que pueda correr sin errores
add_custom_command(TARGET ejemplo_mpi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${MPI_ROOT}/bin/impi.dll"
    $<TARGET_FILE_DIR:ejemplo_mpi>
)

# Version 2 - Rellenar (padding)
add_executable(ejemplo_mpi_v2 main_v2.cpp matrices_v2.cpp)

target_link_libraries(ejemplo_mpi_v2 PRIVATE fmt::fmt ${MPI_LIB}/impi.lib )

add_custom_command(TARGET ejemplo_mpi_v2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${MPI_ROOT}/bin/impi.dll"
    $<TARGET_FILE_DIR:ejemplo_mpi_v2>
)