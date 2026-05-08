package com.programacion.paralela;

import jnr.ffi.LibraryLoader;

import java.nio.Buffer;

public interface FractalDll {

    String LIBRARY_NAME = "libfractal-dll";

    FractalDll INSTANCE  = LibraryLoader.create(FractalDll.class).load(LIBRARY_NAME);

    void julia_simd(double xMin, double yMin, double xMax, double yMax,
                    int width, int height,
                    int maxIteraciones, Buffer pixelBuffer);
}
