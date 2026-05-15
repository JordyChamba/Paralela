#include <iostream>
#include <fmt/core.h>
#include <omp.h>

int main()
{

    // #pragma omp parallel num_threads(4)
    // {

    //     #pragma omp master
    //     {
    //         int threads_count = omp_get_num_threads();
    //         fmt::println("Hello serial world, hello OpenMP!\n");
    //         fmt::println("I have {} thread(s)\n", threads_count);
    //     }

    //     int thread_id = omp_get_thread_num();

    //     fmt::println("My thread ID is {}", thread_id);
    // }

    // #pragma omp parallel
    //     {
    //         int thread_id = omp_get_thread_num();

    //         std::string msg = "";

    // #pragma omp pallel for
    //         {
    //             for (int i = 0; i < thread_id; ++i)
    //             {
    //                 msg = msg + "*";
    //             }
    //         }

    //         fmt::println("Thread {}: {}", thread_id, msg);
    //     }

    int num_elementos = 15;
    // Bucle paralelizado
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < num_elementos; i++) {
        //fmt::println("i {}, thread_id {}", i, omp_get_thread_num());
    }

    // Bloque paralelo
    #pragma omp parallel num_threads(4)
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        int delta = num_elementos / num_threads;
        int start = thread_id * delta;
        int end = (thread_id + 1) * delta;

        if (thread_id==num_threads-1)
        {
            end = num_elementos;
        }
        

        fmt::println("Thread {}: start {}, end {}", thread_id, start, end);

        for(int i = start; i < end; i++)
        {
            //fmt::println("i {}, thread_id {}", i, thread_id);
        }
    }

    #pragma omp parallel num_threads(4)
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        for (int i = thread_id; i < num_elementos; i += 4) {
            
            fmt::println("thread_id {}, index={}", thread_id, i);
        }
    }

    // Bucle paralelo con reducción
    #pragma omp parallel 
    {
        while(true)
        {

        }
    }
    return 0;
}