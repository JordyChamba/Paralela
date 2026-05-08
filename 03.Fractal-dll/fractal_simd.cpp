#include "fractal_simd.h"
#include "palette.h"
#include <cstring>
#include <complex>

#include <immintrin.h> // AVX

std::complex<double> c(-0.7,0.27015);

// **Para que la funcion conserve su nombre y la llamada sea estandar**
// Al declararla como C, se puede acceder desde cualquier lenguaje. 
// Pero los tipos de datos deben ser estandar
extern "C"  __stdcall  
void julia_simd(double x_min, double y_min, double x_max, double y_max, 
    uint32_t width, uint32_t height, 
    int max_iteraciones, uint32_t *pixel_buffer)
{
 
    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;
 
    //(xmin,xmin,xmin,xmin,xmin,xmin,xmin,xmin)
    //(-1.5,-1.5,-1.5,-1.5,-1.5,-1.5,-1.5,-1.5)
    __m256 xmin = _mm256_set1_ps(x_min);
 
    //(ymax,ymax,ymax,ymax,ymax,ymax,ymax,ymax)
    //(1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0)
    __m256 ymax = _mm256_set1_ps(y_max);
 
    __m256 xscale = _mm256_set1_ps(dx); // (dx,dx,dx,dx,dx,dx,dx,dx)
    __m256 yscale = _mm256_set1_ps(dy); // (dy,dy,dy,dy,dy,dy,dy,dy)
 
    __m256 c_real = _mm256_set1_ps(c.real()); //(cx,cx,cx,cx,cx,cx,cx,cx)
    __m256 c_imag = _mm256_set1_ps(c.imag()); //(cy,cy,cy,cy,cy,cy,cy,cy)
 
    __m256 max_norma = _mm256_set1_ps(4.0f); // (4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0)
    __m256 one = _mm256_set1_ps(1.0f);       // (1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0)
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j += 8)
        {
            //(i,i,i,i,i,i,i,i)
            __m256 mx = _mm256_set1_ps(i);
            //(j+7,j+6,j+5,j+4,j+3,j+2,j+1,j)
            __m256 my = _mm256_set_ps(j + 7, j + 6, j + 5, j + 4, j + 3, j + 2, j + 1, j + 0);
 
            // xmin=mx*xscale -->(x0,x1,x2,x3,x4,x5,x6,x7) <--real
            __m256 cr = _mm256_add_ps(xmin, _mm256_mul_ps(mx, xscale));
 
            // ymax -my*yscale -->(y0,y1,y2,y3,y4,y5,y6,y7) <--imag
            __m256 ci = _mm256_sub_ps(ymax, _mm256_mul_ps(my, yscale));
 
            // verificar si los 8 complejos (cr,ci) estan acotados o no
            int iter = 1;
            __m256 mk = _mm256_set1_ps(iter); //(iter,iter,iter,iter,iter,iter,iter,iter)
 
            __m256 zr = cr; // (zr0,zr1,zr2,zr3,zr4,zr5,zr6,zr7)
            __m256 zi = ci; // (zi0,zi1,zi2,zi3,zi4,zi5,zi6,zi7)
 
            while (iter < max_iteraciones)
            {
                // Zm+1 = Zm^2 + c
                __m256 zr2 = _mm256_mul_ps(zr, zr);
                __m256 zi2 = _mm256_mul_ps(zi, zi); // (zr0^2,zr1^2,zr2^2,zr3^2,zr4^2,zr5^2,zr6^2,zr7^2)
                __m256 zrzi = _mm256_mul_ps(zr, zi);
 
                zr = _mm256_add_ps(_mm256_sub_ps(zr2, zi2), c_real);   // zr^2 - zi^2 + cx
                zi = _mm256_add_ps(_mm256_add_ps(zrzi, zrzi), c_imag); // 2*zr*zi
 
                // calcular la norma
                zr2 = _mm256_mul_ps(zr, zr);
 
                zi2 = _mm256_mul_ps(zi, zi);
                __m256 norma2 = _mm256_add_ps(zr2, zi2); // norma^2
 
                // si norma2<=4.0 devuelve 0xFFFFFFFF, sino devuelve 0
                __m256 mask = _mm256_cmp_ps(norma2, max_norma, _CMP_LE_OS);
                mk = _mm256_add_ps(_mm256_and_ps(mask, one), mk); // mascara de los acotados
                if (_mm256_testz_ps(mask, _mm256_set1_ps(-1)))
                { // si no hay ningun acotado, salir del bucle
                    break;
                }
                iter++;
            }
            float d[8];
            _mm256_storeu_ps(d, mk);
 
            for (int it = 0; it < 8; it++)
            {
                int index = (j + it) * width + i;
                if (index < width * height)
                {
                    if (d[it] < max_iteraciones)
                    {
                        int color_index = (int)d[it] % PALETTE_SIZE;
                        pixel_buffer[index] = color_ramp_3[color_index];
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
 