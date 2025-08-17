#include "CLI/CLI.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "Screenshot/Screenshot.hpp"
#include "config.hpp"
#include <cstdint>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <format>
#include <iostream>

static SDL_Window *window{nullptr};

static SDL_Renderer *renderer{nullptr};

static SDL_Texture *texture{nullptr};

static SDL_Surface *pixels{nullptr};

static SDL_Cursor *cursor{nullptr};

static SDL_Rect pixelRect{};

static float mouseX{};
static float mouseY{};

/*
 * TODO: Set window size to full monitor size regardless of whether it's in fullscreen or not.
 * We need to do this, so that reading pixel colors works properly.
*/

/*
 * TODO: convert pixels under cursor to RGB or HEX values.
 * I think HEX values are going to be easier since they're already hex values
 * so, I'd just need to format them into text (#rrggbb) and display them in
 * the terminal.
*/

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    scp::CLI cli(argc, argv);

    auto screenshot{scp::Screenshot::CreateInstance()};
    screenshot->Take();

    SDL_SetAppMetadata("scolorpicker", std::format("{}.{}.{}", SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH).c_str(), "dev.smooll.scolorpicker");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("scolorpicker", 0, 0, SDL_WINDOW_FULLSCREEN, &window, &renderer))
    // if (!SDL_CreateWindowAndRenderer("scolorpicker", 1280, 720, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    SDL_Surface *cursorSurface = SDL_LoadBMP("/home/smooll/Projects/Applications/CPP/scolorpicker/data/cursors/cursor.bmp");
    if (!cursorSurface)
    {
        SDL_Log("Failed to create cursor surface: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    cursor = SDL_CreateColorCursor(cursorSurface, cursorSurface->w / 2, cursorSurface->h / 2);
    if (!cursor)
    {
        SDL_Log("Failed to create cursor: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    SDL_SetCursor(cursor);
    SDL_DestroySurface(cursorSurface);

    texture = screenshot->CreateTexture(renderer);

    pixelRect.w = 1;
    pixelRect.h = 1;

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
                case SDLK_F:
                    SDL_SetWindowFullscreen(window, true);

                    break;
                default:
                    break;
            }

            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            switch (event->button.button)
            {
                case 1:
                    std::cout << pixels->w << ' ' << pixels->h << '\n';

                    std::cout << mouseX << ' ' << mouseY << '\n';

                    return SDL_APP_SUCCESS;
                    // return SDL_APP_CONTINUE;
                default:
                    break;
            }

            break;
        case SDL_EVENT_MOUSE_MOTION:
            break;
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_GetMouseState(&mouseX, &mouseY);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);

    SDL_RenderTexture(renderer, texture, nullptr, nullptr);

    if (!pixels)
    {
        pixels = SDL_RenderReadPixels(renderer, nullptr);
    }

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_DestroySurface(pixels);

    SDL_DestroyTexture(texture);

    SDL_DestroyCursor(cursor);
}
