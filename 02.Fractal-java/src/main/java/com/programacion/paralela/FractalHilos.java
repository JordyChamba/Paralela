package com.programacion.paralela;

public class FractalHilos {
    public final int[] pixel_buffer;
    private final int threadCount;

    public FractalHilos() {
        this.pixel_buffer = new int[FractalParams.WIDTH * FractalParams.HEIGHT];
        this.threadCount = Math.max(1, Runtime.getRuntime().availableProcessors());
    }

    public void juliaThreads(double x_min, double y_min, double x_max, double y_max,
                             int width, int height) {
        double dx = (x_max - x_min) / width;
        double dy = (y_max - y_min) / height;

        int rowsPerThread = (height + threadCount - 1) / threadCount;
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            int startRow = t * rowsPerThread;
            int endRow = Math.min(height, startRow + rowsPerThread);

            threads[t] = new Thread(() -> computeRows(x_min, y_max, dx, dy, width, startRow, endRow));
            threads[t].start();
        }

        for (Thread thread : threads) {
            if (thread != null) {
                try {
                    thread.join();
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    private void computeRows(double x_min, double y_max, double dx, double dy,
                             int width, int startRow, int endRow) {
        for (int j = startRow; j < endRow; j++) {
            for (int i = 0; i < width; i++) {
                double x = x_min + i * dx;
                double y = y_max - j * dy;
                pixel_buffer[j * width + i] = acotado2(x, y);
            }
        }
    }

    public int getThreadCount() {
        return threadCount;
    }

    private int acotado2(double x, double y) {
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
            int index = iter % FractalParams.PALETTE_SIZE;
            return FractalParams.colorRamp3[index];
        }

        return 0xFF000000;
    }
}
