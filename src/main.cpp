#include "CLI/CLI.hpp"
#include "Screenshot/Screenshot.hpp"
#include "version.hpp"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <format>

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;
static SDL_Texture *texture = nullptr;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    scp::CLI cli(argc, argv);

    auto screenshot = scp::Screenshot::CreateInstance();
    screenshot->Take();

    SDL_SetAppMetadata("scolorpicker", std::format("{}.{}.{}", SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH).c_str(), "dev.smooll.scolorpicker");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    // if (!SDL_CreateWindowAndRenderer("scolorpicker", 1280, 720, SDL_WINDOW_FULLSCREEN, &window, &renderer))
    if (!SDL_CreateWindowAndRenderer("scolorpicker", 1280, 720, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    texture = screenshot->CreateTexture(renderer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.key)
            {
                case SDLK_ESCAPE:
                    return SDL_APP_SUCCESS;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // scp::Utils::TestSDL(renderer);

    SDL_RenderClear(renderer);

    SDL_RenderTexture(renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_DestroyTexture(texture);
}
