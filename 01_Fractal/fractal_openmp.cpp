#include "fractal_openmp.h"

#include <complex>
#include <immintrin.h> // avx

#include "palette.h"

#include <omp.h>

extern int max_iteraciones;
extern std::complex<double> c;

uint32_t divergente_openmp(double x, double y) // real e imaginario
{
    int iter = 1;

    double zr = x;
    double zi = y;

    while ((zr * zr + zi * zi) < 4.0 && iter < max_iteraciones)
    {
        double dr = zr * zr - zi * zi + c.real();
        double di = 2.0 * zr * zi + c.imag();

        zr = dr;
        zi = di;

        iter++;
    }

    if (iter < max_iteraciones)
    {
        int index = (iter % PALETTE_SIZE);
        return color_ramp[index];
    }
    return 0xFF000000; // negro
}

void julia_openmp_regiones(double x_min, double x_max, double y_min, double y_max,
                           uint32_t width, uint32_t height,
                           uint32_t *pixel_buffer)
{
    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;

#pragma omp parallel
    {
        int thread_count = omp_get_num_threads();
        int thred_id = omp_get_thread_num();

        // int delta = std::ceil((double)width / thread_count);
        int delta = std::ceil(width * 1.0 / thread_count);

        int start = thred_id * delta;
        int end = std::min(start + delta, (int)width);

        // for (int i = start; i < end - 20; ++i)
        for (int i = start; i < end; ++i) // -20 para dibujar por sectores
        {
            for (int j = 0; j < height; ++j)
            {
                double x = x_min + i * dx;
                double y = y_min + j * dy;

                auto color = divergente_openmp(x, y); // auto es similar a var en java

                pixel_buffer[j * width + i] = color;
            }
        }

        // En este caso no hace falta sincronizar porque cada hebra escribe en
        // una zona diferente del pixel_buffer
        // En este caso se comparte el buffer pero no se pisa la misma zona
    }
}

void julia_openmp_for(double x_min, double x_max, double y_min, double y_max,
                      uint32_t width, uint32_t height,
                      uint32_t *pixel_buffer)
{
    double dx = (x_max - x_min) / width; // variables compartidas
    double dy = (y_max - y_min) / height;

    {

#pragma omp parallel for
        // #pragma omp parallel for default(none) shared(dx, dy, x_min, y_min, width, height, pixel_buffer) // especifica las variables compartidas
        // #pragma omp parallel for schedule(dynamic, 1) collapse(2) // esto reparte mejor la carga, paraleliza ambos for, aveces mejora el rendimiento
        for (int i = 0; i < width; ++i)
        {
            for (int j = 0; j < height; ++j)
            {
                double x = x_min + i * dx;
                double y = y_min + j * dy;

                auto color = divergente_openmp(x, y);
                pixel_buffer[j * width + i] = color;
            }
        }
    }
}

void julia_openmp_for_simd(double x_min, double x_max, double y_min, double y_max,
                           uint32_t width, uint32_t height,
                           uint32_t *pixel_buffer)
{
    double dx = (x_max - x_min) / width; // variables compartidas
    double dy = (y_max - y_min) / height;

    __m256 xmin = _mm256_set1_ps(x_min);
    __m256 ymax = _mm256_set1_ps(y_max);

    __m256 xscale = _mm256_set1_ps(dx);
    __m256 yscale = _mm256_set1_ps(dy);

    __m256 c_real = _mm256_set1_ps(c.real());
    __m256 c_imag = _mm256_set1_ps(c.imag());

    __m256 threshold = _mm256_set1_ps(4.0f);

    __m256 one = _mm256_set1_ps(1.0f);

#pragma omp parallel for
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; j += 8)
        {
            __m256 mx = _mm256_set1_ps(i); // (i,i,i,i,i,i,i,i)
            __m256 my = _mm256_set_ps(j + 7, j + 6, j + 5, j + 4,
                                      j + 3, j + 2, j + 1, j + 0); // (j+7, j+6, ..., j+0)

            // i * dx + x_min
            __m256 cr = _mm256_add_ps(_mm256_mul_ps(mx, xscale), xmin);

            // y = y_max - j * dy
            __m256 ci = _mm256_sub_ps(ymax, _mm256_mul_ps(my, yscale));

            int iter = 1;
            __m256 zr = cr; // inicializar zr = x
            __m256 zi = ci; // inicializar zi = y

            __m256 mk = _mm256_set1_ps(iter); // contador de iteraciones
            // todos empiezan en 1 (1,1,1,1,1,1,1,1)

            while (iter < max_iteraciones)
            {
                // zn+1 = zn * zn + c
                __m256 zr2 = _mm256_mul_ps(zr, zr); // zr * zr
                __m256 zi2 = _mm256_mul_ps(zi, zi); // zi * zi

                __m256 zrzi = _mm256_mul_ps(zr, zi); // zr * zi

                zr = _mm256_add_ps(_mm256_sub_ps(zr2, zi2), c_real);   // zr * zr - zi * zi
                zi = _mm256_add_ps(_mm256_add_ps(zrzi, zrzi), c_imag); // 2.0 * zr * zi

                // norma
                zr2 = _mm256_mul_ps(zr, zr);
                zi2 = _mm256_mul_ps(zi, zi);
                __m256 mag2 = _mm256_add_ps(zr2, zi2);

                __m256 mask = _mm256_cmp_ps(mag2, threshold, _CMP_LT_OS); // comparar si mag2 < 4.0

                // todas las 8 normas deben ser menores que 4 para continuar

                mk = _mm256_add_ps(_mm256_and_ps(mask, one), mk);

                if (_mm256_movemask_ps(mask) == 0)
                // if(_mm256_testz_ps(mask, _mm256_set1_ps(-1)))
                {
                    // todos ya divergieron
                    break; // salir del while
                }
                // incrementar iteracion si no diverge
                iter++;
            }

            // colores
            // desempaquetar
            float d[8];
            _mm256_storeu_ps(d, mk);

            for (int it = 0; it < 8; it++)
            {
                int index = (j + it) * width + i;

                if (index < width * height)
                { // evitar overflow en el borde cuando la imagen no es multiplo de 8
                    if (d[it] < max_iteraciones)
                    {
                        int color_idx = (int)d[it] % PALETTE_SIZE;
                        auto color = color_ramp[color_idx];

                        pixel_buffer[index] = color;
                    }
                    else
                    {
                        pixel_buffer[index] = 0xFF000000; // negro
                    }
                }
            }
        }
    }
}
