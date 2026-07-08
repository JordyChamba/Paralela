#include <SFML/Graphics.hpp>
#include <iostream>
#include <fmt/core.h>
#include <cuda_runtime.h>
#include <cstring>

#define CHECK(expr)                                                                                                         \
  {                                                                                                                         \
    auto internal_error = (expr);                                                                                           \
    if (internal_error != cudaSuccess)                                                                                      \
    {                                                                                                                       \
      fmt::println("{}: {} in {} at line {}", (int)internal_error, cudaGetErrorString(internal_error), __FILE__, __LINE__); \
      exit(EXIT_FAILURE);                                                                                                   \
    }                                                                                                                       \
  }

// Parámetros del Fractal (Recuperados para que compile la función)
double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;
int max_iteraciones = 100;

// dimensión de la imagen
#define WIDTH 1600
#define HEIGHT 900

// tiene que coincidir con PALLETE_SIZE de kernel.cu
#define PALLETE_SIZE 256

uint32_t* host_pixel_buffer = nullptr;
uint32_t* device_pixel_buffer = nullptr;

// Declaración (Prototipo) de tus funciones externas de CUDA
void julia_gpu(double centro_real, double centro_imag, int max_iter,
    double x_min, double y_min, double x_max, double y_max,
    uint32_t width, uint32_t height, uint32_t *pixel_buffer);

void copiar_paleta(unsigned int* h_palette);

double centro_real = -0.7;
double centro_imag = 0.27015;

void generar_paleta(unsigned int* h_palette)
{
    for (int i = 0; i < PALLETE_SIZE; i++)
    {
        double t = (double)i / (double)(PALLETE_SIZE - 1);

        uint8_t r = (uint8_t)(9.0 * (1 - t) * t * t * t * 255);
        uint8_t g = (uint8_t)(15.0 * (1 - t) * (1 - t) * t * t * 255);
        uint8_t b = (uint8_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);

        h_palette[i] = (0xFF << 24) | (b << 16) | (g << 8) | r;
    }
}

int main() {    
    int deviceId = 0;
    cudaSetDevice(deviceId);
    cudaDeviceProp deviceProp;    
    cudaGetDeviceProperties(&deviceProp, deviceId);
    fmt::println("Device: {}", deviceProp.name);    
    fmt::println("Total memory: {} MB", deviceProp.totalGlobalMem / 1024 / 1024);
    
    // INICIALIZAR
    size_t buffer_size = WIDTH * HEIGHT * sizeof(uint32_t);
    host_pixel_buffer = (uint32_t*)malloc(buffer_size);
    std::memset(host_pixel_buffer, 0, buffer_size);

    CHECK(cudaMalloc(&device_pixel_buffer, buffer_size));   

    // paleta de colores: se genera una vez y se copia a la GPU
    unsigned int host_palette[PALLETE_SIZE];
    generar_paleta(host_palette);
    copiar_paleta(host_palette);

    // inicializar UI (CORREGIDO PARA SFML 3)
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Fractal Julia Set");

    // Inicialización directa (CORREGIDO PARA SFML 3: sin método .create())
    sf::Texture texture(sf::Vector2u(WIDTH, HEIGHT));
    sf::Sprite sprite(texture);

    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        fmt::println("No se pudo cargar la fuente arial.ttf");
        return EXIT_FAILURE;
    }
    sf::Text text(font, "Julia Set", 24);
    text.setFillColor(sf::Color::White);
    text.setPosition({10, 10});
    text.setStyle(sf::Text::Bold);

    std::string options = "Options: Up/Down change iterations";
    sf::Text textoptions(font, options, 20);
    textoptions.setStyle(sf::Text::Bold);
    textoptions.setPosition({10, (float)HEIGHT - 40});

    // FPS
    int frames = 0;
    int fps = 0;
    sf::Clock clock;

    while (window.isOpen())
    {
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
                default:
                    break;
                }
            }
        }
        
        std::string mode = "GPU CUDA";

        julia_gpu(centro_real, centro_imag, max_iteraciones,
            x_min, y_min, x_max, y_max,
            WIDTH, HEIGHT, device_pixel_buffer);

        CHECK(cudaMemcpy(host_pixel_buffer, device_pixel_buffer, buffer_size, cudaMemcpyDeviceToHost));
    
        texture.update((const uint8_t *)host_pixel_buffer);

        frames++;

        if (clock.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frames;
            frames = 0;
            clock.restart();
        }
        auto msg = fmt::format("julia: iteraciones: {}. fps: {}, Mode: {}", max_iteraciones, fps, mode);
        text.setString(msg);

        window.clear();
        
        window.draw(sprite);
        window.draw(text);
        window.draw(textoptions);

        window.display();
    }

    free(host_pixel_buffer);
    cudaFree(device_pixel_buffer);

    return 0;
}