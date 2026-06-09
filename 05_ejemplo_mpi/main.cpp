#include <iostream>
#include <fmt/core.h>
#include <mpi.h>

void matrices(int argc, char **argv);

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int nprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    matrices(argc, argv);

    MPI_Finalize();
    return 0;
}