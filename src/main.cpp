#include "SCP_CLI.hpp"

// #define SDL_MAIN_USE_CALLBACKS
// #include <SDL3/SDL.h>
// #include <SDL3/SDL_main.h>

SCP::CLI cli;

int main(int argc, char* argv[])
{
    cli.Initialize(argc, argv);

    return 0;
}
