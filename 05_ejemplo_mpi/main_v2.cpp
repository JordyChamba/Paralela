#include <mpi.h>

void matrices_v2(int argc, char **argv);

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    matrices_v2(argc, argv);

    MPI_Finalize();
    return 0;
}
