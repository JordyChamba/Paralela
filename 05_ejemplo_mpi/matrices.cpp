#include <iostream>
#include <fmt/core.h>
#include <mpi.h>
#include <vector>
#include <cmath>

#define MATRIX_DIH 25

void imprimir_matriz(const std::vector<double>& A_local, int rows, int matrix_dim) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < matrix_dim; j++) {
            std::cout << A_local[i * matrix_dim + j] << " ";
        }
        std::cout << std::endl;
    }
}

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

void matrices(int argc, char **argv)
{
    int nprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int rows_per_rank = std::ceil(MATRIX_DIH * 1.0 / nprocs);
    int padding = rows_per_rank * nprocs - MATRIX_DIH;

    if (rank == 0)
    {
        std::vector<double> A(MATRIX_DIH * MATRIX_DIH);
        std::vector<double> B(MATRIX_DIH);
        std::vector<double> X(MATRIX_DIH, 0.0);

        for (int i = 0; i < MATRIX_DIH; i++) {
            for (int j = 0; j < MATRIX_DIH; j++) {
                A[i * MATRIX_DIH + j] = i;
            }
            B[i] = 1.0;
        }

        fmt::print("MATRIX_DIM: {}, nprocs: {}, rows_per_rank: {}, padding: {}\n", MATRIX_DIH, nprocs, rows_per_rank, padding);

        int filas_enviadas = rows_per_rank;
        for (int i = 1; i < nprocs; i++)
        {
            int fila = rows_per_rank;
            if (i == nprocs - 1) {
                fila = rows_per_rank - padding;
            }

            std::vector<int> data = {MATRIX_DIH, fila};
            MPI_Send(data.data(), 2, MPI_INT, i, 0, MPI_COMM_WORLD);

            if (fila > 0) {
                MPI_Send(&A[filas_enviadas * MATRIX_DIH], fila * MATRIX_DIH, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
                MPI_Send(B.data(), MATRIX_DIH, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
            }
            filas_enviadas += rows_per_rank;
        }

        multiplicar_matriz_vector(A, B, X, rows_per_rank, MATRIX_DIH);

        int posicion_recibo = rows_per_rank;
        for (int i = 1; i < nprocs; i++)
        {
            int fila = rows_per_rank;
            if (i == nprocs - 1) {
                fila = rows_per_rank - padding;
            }

            if (fila > 0) {
                MPI_Recv(X.data() + posicion_recibo, fila, MPI_DOUBLE, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            posicion_recibo += rows_per_rank;
        }

        fmt::print("RANK_{}, resultado parcial:\n", rank);
        imprimir_vector(X);
    }
    else
    {
        std::vector<int> data_rec(2);
        MPI_Recv(data_rec.data(), 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        int matrix_dim = data_rec[0];
        int rows = data_rec[1];

        if (rows > 0) {
            std::vector<double> A_local(rows * matrix_dim);
            std::vector<double> b_local(matrix_dim);

            MPI_Recv(A_local.data(), rows * matrix_dim, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(b_local.data(), matrix_dim, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            std::vector<double> X_local(rows);
            multiplicar_matriz_vector(A_local, b_local, X_local, rows, matrix_dim);

            MPI_Send(X_local.data(), rows, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
        }
    }
}