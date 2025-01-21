#include "SDL3/SDL_mouse.h"
#include <SCP/SCP_CLI.hpp>
#include <SCP/SCP_Window.hpp>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SCP::CLI cli;
SCP::Window window;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    cli.Initialize(argc, argv);

    window.Initialize();
    window.Create();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        default:
            return SDL_APP_CONTINUE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    window.Iterate();

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) { }
