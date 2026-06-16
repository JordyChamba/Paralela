#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
#include <complex>
#include <optional>

#include <fmt/core.h>
#include <SFML/Graphics.hpp>
#include <mpi.h>

#include "fractal_mpi.h"
#include "draw_text.h"
#include "palette.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace arial_ttf
{
    extern size_t data_len;
    extern unsigned char data[];
}

int max_iteraciones = 10;
int mode = 1;
double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;
uint32_t *pixel_buffer = nullptr;
uint32_t *texture_buffer = nullptr;
int running = 1;
int delta_global = 0;
int row_start_global = 0;
int row_end_global = 0;

std::complex<double> c(-0.71, 0.27015);

#define ANCHO 1600
#define ALTO 900

std::string machine_name()
{
    std::string mname = "";
#ifdef _WIN32
    char hostname[256];
    DWORD size = sizeof(hostname);
    GetComputerNameA(hostname, &size);
    mname = hostname;
#endif
    return mname;
}

void setup_ui(int nprocs)
{
    texture_buffer = new uint32_t[ANCHO * ALTO];
    std::memset(texture_buffer, 0, ANCHO * ALTO * sizeof(uint32_t));

    // Calcular filas reales de cada rank — ANTES de usarlo
    std::vector<int> filas_por_rank(nprocs);
    for (int r = 0; r < nprocs; r++)
    {
        int rs = r * delta_global;
        int re = rs + delta_global;
        if (re > ALTO)
            re = ALTO;
        filas_por_rank[r] = re - rs;
    }

    // Buffer temporal para recibir porciones — ANTES de usarlo
    std::vector<uint32_t> recv_buf(ANCHO * delta_global);

    uint32_t color_linea = 0xFF00FFFF;

    sf::RenderWindow window(sf::VideoMode({ANCHO, ALTO}), "Fractal MPI");
#ifdef _WIN32
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif

    sf::Texture texture({ANCHO, ALTO});
    sf::Sprite sprite(texture);

    const sf::Font font(arial_ttf::data, arial_ttf::data_len);

    sf::Text text(font, "Fractal", 24);
    text.setFillColor(sf::Color::White);
    text.setPosition({10, 10});
    text.setStyle(sf::Text::Bold);

    std::string options = "[UP] Mas iteraciones  [DOWN] Menos iteraciones";
    sf::Text textOptions(font, options, 18);
    textOptions.setFillColor(sf::Color::White);
    textOptions.setStyle(sf::Text::Bold);
    textOptions.setPosition({10, window.getView().getSize().y - 40});

    int frames = 0;
    int fps = 0;
    sf::Clock clock;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                running = 0;
                window.close();
            }
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

        std::vector<int> estado = {max_iteraciones, running};
        MPI_Bcast(estado.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);

        if (running == 0)
            break;

        // Rank 0 calcula su porción
        julia_mpi(x_min, y_min, x_max, y_max, ANCHO, ALTO,
                  row_start_global, row_end_global, pixel_buffer);

        // Copiar rank 0 al texture_buffer
        int filas_rank0 = filas_por_rank[0];
        std::memcpy(texture_buffer, pixel_buffer,
                    ANCHO * filas_rank0 * sizeof(uint32_t));

        // Línea al final de la porción del rank 0
        int fila_linea_r0 = filas_rank0 - 1;
        for (int x = 0; x < ANCHO; x++)
            texture_buffer[fila_linea_r0 * ANCHO + x] = color_linea;

        // Recibir porciones de los demás ranks
        for (int r = 1; r < nprocs; r++)
        {
            int filas_r = filas_por_rank[r];
            MPI_Recv(
                recv_buf.data(),
                ANCHO * filas_r,
                MPI_UNSIGNED,
                r,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);

            uint32_t *destino = texture_buffer + r * delta_global * ANCHO;
            std::memcpy(destino, recv_buf.data(),
                        ANCHO * filas_r * sizeof(uint32_t));

            // Línea en la primera fila de cada rank worker
            for (int x = 0; x < ANCHO; x++)
                destino[x] = color_linea;
        }

        texture.update((const uint8_t *)texture_buffer);

        frames++;
        if (clock.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frames;
            frames = 0;
            clock.restart();
        }

        auto msg = fmt::format("Julia | Iteraciones: {}  FPS: {}  Mode: {} Name: {}", max_iteraciones, fps, mode, machine_name());
        text.setString(msg);

        window.clear();
        window.draw(sprite);
        window.draw(text);
        window.draw(textOptions);
        window.display();
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int nprocs, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    init_freetype();

    delta_global = (int)std::ceil(1.0 * ALTO / nprocs);
    row_start_global = rank * delta_global;
    row_end_global = row_start_global + delta_global;
    if (row_end_global > ALTO)
        row_end_global = ALTO;

    int filas_reales = row_end_global - row_start_global;
    pixel_buffer = new uint32_t[ANCHO * filas_reales];
    std::memset(pixel_buffer, 0, ANCHO * filas_reales * sizeof(uint32_t));

    fmt::print("Rank {}: rows {} to {}\n", rank, row_start_global, row_end_global);

    if (rank == 0)
    {
        setup_ui(nprocs);
    }
    else
    {
        while (true)
        {
            std::vector<int> estado(2, 0);
            MPI_Bcast(estado.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);

            max_iteraciones = estado[0];
            running = estado[1];

            if (running == 0)
            {
                fmt::print("Rank {}: shutdown\n", rank);
                break;
            }

            julia_mpi(x_min, y_min, x_max, y_max, ANCHO, ALTO,
                      row_start_global, row_end_global, pixel_buffer);

            MPI_Send(
                pixel_buffer,
                ANCHO * filas_reales,
                MPI_UNSIGNED,
                0,
                0,
                MPI_COMM_WORLD);
        }
    }

    delete[] pixel_buffer;
    if (rank == 0)
        delete[] texture_buffer;

    MPI_Finalize();
    return 0;
}