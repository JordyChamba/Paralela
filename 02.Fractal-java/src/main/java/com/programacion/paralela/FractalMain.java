package com.programacion.paralela;

import org.lwjgl.*;
import org.lwjgl.glfw.*;
import org.lwjgl.opengl.*;
import org.lwjgl.system.*;

import java.nio.*;

import static org.lwjgl.glfw.Callbacks.*;
import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.opengl.GL11.*;
import static org.lwjgl.system.MemoryStack.*;
import static org.lwjgl.system.MemoryUtil.*;

public class FractalMain {

    // The window handle
    private int textureID;
    private long window;

    FractalCpu fractalCpu;
    FractalSimd fractalSimd;

    private IntBuffer pixelBuffer;

    int modo = 1;
    String title = "Julia";

    public FractalMain() {
        fractalCpu = new FractalCpu();
        fractalSimd = new FractalSimd();

        pixelBuffer = BufferUtils.createIntBuffer(FractalParams.WIDTH*FractalParams.HEIGHT);
    }

    public void run() {
        System.out.println("Fractal Julia " + Version.getVersion());

        init();
        loop();

        // Free the window callbacks and destroy the window
        glfwFreeCallbacks(window);
        glfwDestroyWindow(window);

        // Terminate GLFW and free the error callback
        glfwTerminate();
        glfwSetErrorCallback(null).free();
    }

    private void init() {
        GLFWErrorCallback.createPrint(System.err).set();

        if (!glfwInit()) throw new IllegalStateException("Unable to initialize GLFW");

        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        // Create the window
        window = glfwCreateWindow(FractalParams.WIDTH, FractalParams.HEIGHT, "Julia Set", 0, 0);

        if (window == NULL) throw new RuntimeException("Failed to create the GLFW window");

        glfwSetKeyCallback(window, (window, key, scancode, action, mods) -> {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) glfwSetWindowShouldClose(window, true);

            if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
                FractalParams.maxIteraciones += 10;
            }

            if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
                FractalParams.maxIteraciones -= 10;

                if (FractalParams.maxIteraciones < 0) {
                    FractalParams.maxIteraciones = 10;
                }
            }

            if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
                System.out.println("Modo java CPU");
                modo = 1;
                title = "Julia";
            }

            if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
                System.out.println("Modo C/C++ SIMD");
                modo = 2;
                title = "Simd";
            }
        });

        GLFWVidMode vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(window, (vidmode.width() - FractalParams.WIDTH) / 2, (vidmode.height() - FractalParams.HEIGHT) / 2);

        glfwMakeContextCurrent(window);

        GL.createCapabilities();
        GL.createCapabilitiesWGL();

        // -- version OpenGL
        String version = GL11.glGetString(GL11.GL_VERSION);
        String vendor = GL11.glGetString(GL_VENDOR);
        String renderer = GL11.glGetString(GL_RENDERER);

        System.out.println("OpenGL version: " + version);
        System.out.println("OpenGL vendor: " + vendor);
        System.out.println("OpenGL renderer: " + renderer);

        // - conf. proyección

        GL11.glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_TEXTURE_2D);
        glLoadIdentity();

        glfwSwapInterval(1);
        glfwShowWindow(window);

        setupTexture();
    }

    private void setupTexture() {
        textureID = glGenTextures();
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                FractalParams.WIDTH, FractalParams.HEIGHT,
                0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    private void loop() {
        GL.createCapabilities();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            paint();
            int fps = fractalCpu.fpsCounter.update();
            glfwSetWindowTitle(window, "Fractal " + title + " | FPS: " + fps + " | Iteraciones: " + FractalParams.maxIteraciones);

            glfwSwapBuffers(window);

            glfwPollEvents();
        }
    }

    public void paint() {



        pixelBuffer.clear();

        if (modo == 1) {
            fractalCpu.juliaSerial2(FractalParams.xMin, FractalParams.yMin, FractalParams.xMax,
                    FractalParams.yMax, FractalParams.WIDTH, FractalParams.HEIGHT);
            pixelBuffer.put(fractalCpu.pixel_buffer);
        } else if (modo == 2) {
            fractalSimd.juliaSimd();
            pixelBuffer.put(fractalSimd.pixelBuffer.asIntBuffer());
        }

        pixelBuffer.flip();


        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                FractalParams.WIDTH, FractalParams.HEIGHT,
                0, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);

        glBegin(GL_QUADS);
        {
            glTexCoord2f(0, 0);
            glVertex2d(-1, -1);

            glTexCoord2f(0, 1);
            glVertex2d(-1, 1);

            glTexCoord2f(1, 1);
            glVertex2d(1, 1);

            glTexCoord2f(1, 0);
            glVertex2d(1, -1);
        }
        glEnd();
    }

    // Declarar un metodo como native implicara que estara esrito en C,
    // y que probablemente se usara a traves de un dll. Pero esta implementacion es demasiado compleja
    // public native void julia_simd();

    public static void main(String[] args) {
        new FractalMain().run();
    }
}
