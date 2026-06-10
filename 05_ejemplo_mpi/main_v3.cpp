#include <mpi.h>

int matrices_v3(int argc, char **argv);

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    matrices_v3(argc, argv);

    MPI_Finalize();
    return 0;
}
