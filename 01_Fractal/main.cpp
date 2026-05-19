#include <iostream>
#include <complex>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <fmt/core.h>
#include <fmt/format.h>

#include "arial.ttf.h"

#include "fractal_serial.h"
#include "fractal_simd.h"
#include "fractal_openmp.h"

#include <omp.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Parametros
double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

int max_iteraciones = 100;

std::complex<double> c(-0.7, 0.27015); // Esta debe ser global para fractal_serial.cpp

// dimension de la imagen
#define WIDTH 1600
#define HEIGHT 900

uint32_t *pixel_buffer = nullptr; // WxH
// uint8_t* image_data = nullptr; // WxHx4 (RGBA)

enum class runtime_type
{
    SERIAL_1 = 0,
    SERIAL_2,
    SIMD,
    OPENMP_REGIONES,
    OPENMP_FOR,
    OPENMP_FOR_SIMD
};

int main()
{

    int thread_count;

#pragma omp parallel
    {
#pragma omp single
        {
            thread_count = omp_get_num_threads();
        }
    }
    // - Inicializar
    pixel_buffer = new uint32_t[WIDTH * HEIGHT];

    runtime_type r_type = runtime_type::SERIAL_1;

    julia_serial_1(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
    // julia_serial_1(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
    // for(int i = 0; i < WIDTH * HEIGHT; ++i)
    // {
    //     pixel_buffer[i] = 0xFFFF0000; //
    // }

    // inicializar SFML
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    // Create the main window
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "SFML window");

    // #ifdef _WIN32
    //     HWND hwnd = window.getSystemHandle();
    //     ShowWindow(hwnd, SW_MAXIMIZE);
    // #endif

    sf::Texture texture({WIDTH, HEIGHT});
    texture.update((const uint8_t *)pixel_buffer);

    sf::Sprite sprite(texture);
    // escalar el sprite para que ocupe toda la ventana

    // TEXTOS

    const sf::Font font(arial_ttf, sizeof(arial_ttf));
    sf::Text text(font, "Hello SFML", 50);
    //  Create a graphical text to display
    // const sf::Font font("arial.ttf");
    // sf::Text text(font, "Fractal Julia", 24);
    // text.setFillColor(sf::Color::White);
    // text.setPosition({10.f, 10.f});
    // text.setStyle(sf::Text::Bold);

    std::string options = "Options: [1] Serial 1, [2] Serial 2 [3] SIMD [4] OpenMP Regiones [5] OpenMP For [6] OpenMP For+SIMD\n "
                          "Up/Down: Increase/Decrease iterations";
    sf ::Text textOptions(font, options, 24);
    textOptions.setFillColor(sf::Color::White);
    textOptions.setStyle(sf::Text::Bold);
    textOptions.setPosition({10, window.getSize().y - 40.f});

    // FPS
    int frames = 0;
    int fps = 0;
    sf::Clock clockFrames;

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();

            else if (event->is<sf::Event::KeyReleased>())
            {
                auto evt = event->getIf<sf::Event::KeyReleased>();

                switch (evt->scancode)
                {
                case sf::Keyboard::Scan::Up:
                    max_iteraciones += 10;
                    break;
                case sf::Keyboard::Scan::Down:
                    max_iteraciones -= 10;
                    if (max_iteraciones < 10)
                        max_iteraciones = 10;
                    break;
                case sf::Keyboard::Scan::Num1:
                    r_type = runtime_type::SERIAL_1;
                    break;
                case sf::Keyboard::Scan::Num2:
                    r_type = runtime_type::SERIAL_2;
                    break;
                case sf::Keyboard::Scan::Num3:
                    r_type = runtime_type::SIMD;
                    break;

                case sf::Keyboard::Scan::Num4:
                    r_type = runtime_type::OPENMP_REGIONES;
                    break;
                case sf::Keyboard::Scan::Num5:
                    r_type = runtime_type::OPENMP_FOR;
                    break;
                case sf::Keyboard::Scan::Num6:
                    r_type = runtime_type::OPENMP_FOR_SIMD;
                    break;
                }

                std::memset(pixel_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t)); // limpiar el buffer de pixeles
            }

            std::cout << "Max iteraciones: " << max_iteraciones << std::endl;
        }

        std::string mode = "";

        if (r_type == runtime_type::SERIAL_1)
        {
            mode = "SERIAL_1";
            julia_serial_1(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::SERIAL_2)
        {
            mode = "SERIAL_2";
            julia_serial_2(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::SIMD)
        {
            mode = "SIMD";
            julia_simd(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::OPENMP_REGIONES)
        {
            mode = fmt::format("OPENMP_REGIONES (Threads: {})", thread_count);
            julia_openmp_regiones(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::OPENMP_FOR)
        {
            mode = fmt::format("OPENMP_FOR (Threads: {})", thread_count);
            julia_openmp_for(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
        }
        else if (r_type == runtime_type::OPENMP_FOR_SIMD)
        {
            mode = fmt::format("OPENMP_FOR_SIMD (Threads: {})", thread_count);
            julia_openmp_for_simd(x_min, x_max, y_min, y_max, WIDTH, HEIGHT, pixel_buffer);
        }

        texture.update((const uint8_t *)pixel_buffer);

        // contar FPS
        frames++;
        if (clockFrames.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frames;
            frames = 0;
            clockFrames.restart();
        }
        // actualizar titulo
        auto msg = fmt::format("Julia Set: Iteraciones :{}, FPS: {}, Mode: {}", max_iteraciones, fps, mode);
        text.setString(msg);

        // Clear screen
        window.clear();
        // Draw the string
        {
            window.draw(sprite);
            window.draw(text);
            window.draw(textOptions);
        }

        // Update the window
        window.display();
    }
}