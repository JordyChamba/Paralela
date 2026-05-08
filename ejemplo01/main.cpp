#include <iostream>
#include <fmt/core.h>


int main()
{

    const char* valor = "xyz";

    std::printf("Hola, mundo: %s!\n", valor);

    fmt::println("Hola mundo con fmt: {}!", valor); 
    //namespace :: nombrefuncion -> fmt :: println
    // el fmt entiende el tipo de dato por mas que se cambie

    return 0;
}