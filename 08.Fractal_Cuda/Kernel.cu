#include <cstdint>
#include <cmath>
#include <cuda_runtime.h>

#define PALLETE_SIZE 256

__constant__ unsigned int color_ramp[PALLETE_SIZE];

void copiar_paleta(unsigned int* h_palette)
{
    cudaMemcpyToSymbol(color_ramp, h_palette, PALLETE_SIZE * sizeof(unsigned int));
}

__device__ uint32_t color_from_iter(int iter, int max_iter)
{
    if (iter == max_iter)
        return 0xFF000000;

    int index = (int)((long long)iter * (PALLETE_SIZE - 1) / max_iter);
    return color_ramp[index];
}

__global__ void julia_kernel(double centro_real, double centro_imag, int max_iter,
    double x_min, double y_min, double x_max, double y_max,
    uint32_t width, uint32_t height, uint32_t *pixel_buffer)
{
    int px = blockIdx.x * blockDim.x + threadIdx.x;
    if (px >= (int)(width * height))
        return;

    int x = px % width;
    int y = px / width;

    double real = x_min + (x_max - x_min) * x / (double)width;
    double imag = y_min + (y_max - y_min) * y / (double)height;

    int iter = 0;
    while (real * real + imag * imag <= 4.0 && iter < max_iter)
    {
        double temp_real = real * real - imag * imag + centro_real;
        imag = 2.0 * real * imag + centro_imag;
        real = temp_real;
        iter++;
    }

    pixel_buffer[px] = color_from_iter(iter, max_iter);
}

void julia_gpu(double centro_real, double centro_imag, int max_iter,
    double x_min, double y_min, double x_max, double y_max,
    uint32_t width, uint32_t height, uint32_t *pixel_buffer)
{
    int threads_per_block = 1024;
    int blocks_per_grid = (int)std::ceil((width * height) * 1.0 / threads_per_block);

    julia_kernel<<<blocks_per_grid, threads_per_block>>>(
        centro_real, centro_imag, max_iter,
        x_min, y_min, x_max, y_max,
        width, height, pixel_buffer);
}
