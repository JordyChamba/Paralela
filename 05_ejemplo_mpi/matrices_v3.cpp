#include <iostream>
#include <fmt/core.h>
#include <mpi.h>
#include <vector>
#include <cmath>

#define MATRIX_DIH 25

void imprimir_vector(const std::vector<double>& v) {
    for (size_t i = 0; i < v.size(); i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
}

void multiplicar_matriz_vector(const std::vector<double>& A,
                               const std::vector<double>& b,
                               std::vector<double>& X,
                               int rows,
                               int cols)
{
    for (int i = 0; i < rows; i++) {
        double sum = 0.0;
        for (int j = 0; j < cols; j++) {
            sum += A[i * cols + j] * b[j];
        }
        X[i] = sum;
    }
}

int matrices_v3(int argc, char **argv)
{
    int nprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int rows_per_rank = std::ceil(MATRIX_DIH * 1.0 / nprocs);
    int padded_rows = rows_per_rank * nprocs;
    int padding = padded_rows - MATRIX_DIH;

    int dims[2] = {MATRIX_DIH, rows_per_rank};

    MPI_Bcast(dims, 2, MPI_INT, 0, MPI_COMM_WORLD);
    int matrix_dim = dims[0];
    int rows = dims[1];

    std::vector<double> B(matrix_dim);
    if (rank == 0)
    {
        for (int i = 0; i < matrix_dim; i++) {
            B[i] = 1.0;
        }
    }

    MPI_Bcast(B.data(), matrix_dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    std::vector<double> A_local(rows * matrix_dim);

    if (rank == 0)
    {
        std::vector<double> A(padded_rows * matrix_dim, 0.0);
        for (int i = 0; i < MATRIX_DIH; i++) {
            for (int j = 0; j < MATRIX_DIH; j++) {
                A[i * matrix_dim + j] = static_cast<double>(i);
            }
        }

        fmt::print("MATRIX_DIM: {}, nprocs: {}, rows_per_rank: {}, padding: {}\n",
                   MATRIX_DIH, nprocs, rows_per_rank, padding);

        MPI_Scatter(A.data(), rows * matrix_dim, MPI_DOUBLE,
                    A_local.data(), rows * matrix_dim, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Scatter(nullptr, rows * matrix_dim, MPI_DOUBLE,
                    A_local.data(), rows * matrix_dim, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);
    }

    std::vector<double> X_local(rows);
    multiplicar_matriz_vector(A_local, B, X_local, rows, matrix_dim);

    std::vector<double> X(padded_rows);

    MPI_Gather(X_local.data(), rows, MPI_DOUBLE,
               X.data(), rows, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        X.resize(MATRIX_DIH);
        fmt::print("RANK_{}, resultado parcial:\n", rank);
        imprimir_vector(X);
    }

    return 0;
}
