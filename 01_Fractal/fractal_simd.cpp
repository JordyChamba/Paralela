#include "fractal_simd.h"

#include <complex>
#include <immintrin.h> // avx

#include "palette.h"

extern int max_iteraciones;
extern std::complex<double> c;

void julia_simd(double x_min, double x_max, double y_min, double y_max,
                uint32_t width, uint32_t height,
                uint32_t *pixel_buffer)
{

    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;

    // (x_min, x_min, x_min, x_min, x_min, x_min, x_min, x_min) 8 veces
    __m256 xmin = _mm256_set1_ps(x_min); // Lo que hace es al vector de 8 floats
    // (y_max, y_max, y_max, y_max, y_max, y_max, y_max, y_max) 8 veces
    __m256 ymax = _mm256_set1_ps(y_max);

    __m256 xscale = _mm256_set1_ps(dx); // delta_x 8 veces: dx, dx ...
    __m256 yscale = _mm256_set1_ps(dy); // 8 veces dy

    // repetir el centro c 8 veces
    __m256 c_real = _mm256_set1_ps(c.real()); // (cr, cr, cr, cr, cr, cr, cr, cr, cr)
    __m256 c_imag = _mm256_set1_ps(c.imag()); // (ci, ci, ci, ci, ci, ci, ci, ci, ci)

    // para comparar que la norma sea menor que 4
    __m256 threshold = _mm256_set1_ps(4.0f); // (4,4,4,4,4,4,4,4)

    __m256 one = _mm256_set1_ps(1.0f); // (1,1,1,1,1,1,1,1)

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

            // int index0 = (j + 0) * width + i; // estos son los 8 pixeles que empaquetamos
            // int index1 = (j + 1) * width + i;
            // int index2 = (j + 2) * width + i;
            // int index3 = (j + 3) * width + i;
            // int index4 = (j + 4) * width + i;
            // int index5 = (j + 5) * width + i;
            // int index6 = (j + 6) * width + i;
            // int index7 = (j + 7) * width + i;

            // int color0 = color_ramp[(int)d[0] % PALETTE_SIZE];
            // int color1 = color_ramp[(int)d[1] % PALETTE_SIZE];
            // int color2 = color_ramp[(int)d[2] % PALETTE_SIZE];
            // int color3 = color_ramp[(int)d[3] % PALETTE_SIZE];
            // int color4 = color_ramp[(int)d[4] % PALETTE_SIZE];
            // int color5 = color_ramp[(int)d[5] % PALETTE_SIZE];
            // int color6 = color_ramp[(int)d[6] % PALETTE_SIZE];
            // int color7 = color_ramp[(int)d[7] % PALETTE_SIZE];

            // pixel_buffer[index0] = color0;
            // ...

            for (int it = 0; it < 8; it++)
            {
                int index = (j + it) * width + i;

                if (index; width * height)
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