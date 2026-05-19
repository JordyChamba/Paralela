#include "fractal_serial.h"

#include <complex>

#include "palette.h"

extern int max_iteraciones;
extern std::complex<double> c;

uint32_t divergente_1(std::complex<double> z0)
{

    // dato: c, Z0
    // Zn+1 = Zn^2 + c

    int iter = 1;

    std::complex<double> z = z0;

    while (std::abs(z) < 2.0 && iter < max_iteraciones)
    {
        z = z * z + std::complex<double>(-0.7, 0.27015);
        iter++;
    }

    if (iter < max_iteraciones)
    {
        // return 0xFF0000FF; // rojo
        int index = (iter % PALETTE_SIZE);
        return color_ramp[index];
    }
    return 0xFF000000; // negro
}

uint32_t divergente_2(double x, double y) // real e imaginario
{

    // dato: c, Z0
    // Zn+1 = Zn^2 + c

    int iter = 1;

    // std::complex<double> z = z0;
    double zr = x;
    double zi = y;

    while ((zr * zr + zi * zi) < 4.0 && iter < max_iteraciones)
    {
        // z = z * z + std::complex<double>(-0.7, 0.27015);
        double dr = zr * zr - zi * zi + c.real();
        double di = 2.0 * zr * zi + c.imag();

        zr = dr;
        zi = di;

        iter++;
    }

    if (iter < max_iteraciones)
    {
        // return 0xFF0000FF; // rojo
        int index = (iter % PALETTE_SIZE);
        return color_ramp[index];
    }
    return 0xFF000000; // negro
}

void julia_serial_1(double x_min, double x_max, double y_min, double y_max,
                    uint32_t width, uint32_t height,
                    uint32_t *pixel_buffer)
{
    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            double x = x_min + i * dx;
            double y = y_min + j * dy;

            std::complex<double> z(x, y);

            auto color = divergente_1(z); // auto es similar a var en java

            pixel_buffer[j * width + i] = color;
        }
    }
}

void julia_serial_2(double x_min, double x_max, double y_min, double y_max,
                    uint32_t width, uint32_t height,
                    uint32_t *pixel_buffer)
{
    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            double x = x_min + i * dx;
            double y = y_min + j * dy;

            // std::complex<double> z(x, y);

            auto color = divergente_2(x, y); // auto es similar a var en java

            pixel_buffer[j * width + i] = color;
        }
    }
}