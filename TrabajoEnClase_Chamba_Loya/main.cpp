#include <iostream>
#include <fmt/core.h>
#include <omp.h>
#include <vector>

int main()
{
    constexpr int num = 8;
    std::vector<float> a1 = {1,2,3,4,5,6,7,8};
    std::vector<float> a2 = {8,7,6,5,4,3,2,1};

    float resultado1 = 0.0f;
    float resultado3 = 0.0f;
    float resultado4 = 0.0f;
    float resultado5 = 0.0f;
    float resultado6 = 0.0f;

    // Producto escalar con operaciones vectoriales
    for (int i = 0; i < num; i++) {
        resultado1 += a1[i] * a2[i];
    }
    fmt::print("Resultado secuencial: {}\n", resultado1);

    // Producto escalar con OpenMP y secciones
    float parcial1 = 0, parcial2 = 0, parcial3 = 0, parcial4 = 0;

    #pragma omp parallel sections
    {
        #pragma omp section
        for (int i = 0; i < num/4; i++) {
            parcial1 += a1[i] * a2[i];
            resultado3 = parcial1;
            fmt::print("Parcial 1: {}\n", resultado3);
        }

        #pragma omp section
        for (int i = num/4; i < num/2; i++) {
            parcial2 += a1[i] * a2[i];
            resultado4 = parcial2;
            fmt::print("Parcial 2: {}\n", resultado4);
        }

        #pragma omp section
        for (int i = num/2; i < 3*num/4; i++) {
            parcial3 += a1[i] * a2[i];
            resultado5 = parcial3;
            fmt::print("Parcial 3: {}\n", resultado5);

            fmt::print("Parcial 3: {}\n", parcial3);
        }

        #pragma omp section
        for (int i = 3*num/4; i < num; i++) {
            parcial4 += a1[i] * a2[i];
            resultado6 = parcial4;
            fmt::print("Parcial 4: {}\n", resultado6);
        }
    }

    float resultado2 = parcial1 + parcial2 + parcial3 + parcial4;
    fmt::print("Resultado paralelo: {}\n", resultado2);

    return 0;
}