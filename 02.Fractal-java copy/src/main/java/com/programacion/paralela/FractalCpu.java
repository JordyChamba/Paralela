package com.programacion.paralela;

public class FractalCpu {

    public int pixel_buffer[];

    public FPSCounter fpsCounter;

    public FractalCpu() {
        this.pixel_buffer = new int[FractalParams.WIDTH * FractalParams.HEIGHT];
        this.fpsCounter = new FPSCounter();
    }

    int acotado2(double x, double y) {

        // dados c y z0 calculamos zn+1 = zn^2+c

        int iter = 1;

        double zr = x;
        double zi = y;

        while (iter < FractalParams.maxIteraciones && (zr * zr + zi * zi) < 4.0) {
            double dr = zr * zr - zi * zi + FractalParams.cReal;
            double di = 2.0 * zr * zi + FractalParams.cImag;

            zr = dr;
            zi = di;

            iter++;
        }

        if (iter < FractalParams.maxIteraciones) {
            // la norma es > 2
            // return 0xFF0000FF; // ROJO
            int index = iter % FractalParams.PALETTE_SIZE;
            return FractalParams.colorRamp[index];
        }

        return 0xFF000000; // NEGRO
    }

    // Segunda versión
    void juliaSerial2(double x_min, double y_min, double x_max, double y_max, int width, int height) {
        double dx = (x_max - x_min) / width;
        double dy = (y_max - y_min) / height;

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                // z = x+yi = (x,y)
                double x = x_min + i * dx;
                double y = y_max - j * dy;

                var color = acotado2(x, y); // auto = var (java)

                pixel_buffer[j * width + i] = color;
            }
        }
    }

}
