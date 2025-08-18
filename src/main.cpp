#include "CLI/CLI.hpp"
#include "Screenshooter/Screenshooter.hpp"
#include "config.hpp"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <format>
#include <iostream>

struct AppState
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Texture *screenshot;
    SDL_Surface *screenshotPixels;

    SDL_Cursor *cursor;

    float mouseX;
    float mouseY;
};

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
    AppState *appState = static_cast<AppState*>(SDL_calloc(1, sizeof(AppState)));
    *appstate = appState;

    scp::CLI cli(argc, argv);

    auto screenshooter{scp::Screenshooter::CreateInstance()};
    screenshooter->Take();

    SDL_SetAppMetadata("scolorpicker", std::format("{}.{}.{}", SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH).c_str(), "dev.smooll.scolorpicker");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("scolorpicker", 0, 0, SDL_WINDOW_FULLSCREEN, &appState->window, &appState->renderer))
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

    appState->cursor = SDL_CreateColorCursor(cursorSurface, cursorSurface->w / 2, cursorSurface->h / 2);
    if (!appState->cursor)
    {
        SDL_Log("Failed to create cursor: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    SDL_SetCursor(appState->cursor);
    SDL_DestroySurface(cursorSurface);

    appState->screenshot = screenshooter->CreateTexture(appState->renderer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppState *appState = static_cast<AppState*>(appstate);

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
                    SDL_SetWindowFullscreen(appState->window, true);

                    break;
                default:
                    break;
            }

            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            switch (event->button.button)
            {
                case 1:
                    std::cout << appState->screenshotPixels->w << ' ' << appState->screenshotPixels->h << '\n';

                    std::cout << appState->mouseX << ' ' << appState->mouseY << '\n';

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
    AppState *appState = static_cast<AppState*>(appstate);

    SDL_GetMouseState(&appState->mouseX, &appState->mouseY);

    SDL_SetRenderDrawColor(appState->renderer, 0, 0, 0, 255);

    SDL_RenderClear(appState->renderer);

    SDL_RenderTexture(appState->renderer, appState->screenshot, nullptr, nullptr);

    if (!appState->screenshotPixels)
    {
        appState->screenshotPixels = SDL_RenderReadPixels(appState->renderer, nullptr);
    }

    SDL_RenderPresent(appState->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate)
    {
        AppState *appState = static_cast<AppState*>(appstate);

        SDL_DestroySurface(appState->screenshotPixels);

        SDL_DestroyTexture(appState->screenshot);

        SDL_DestroyCursor(appState->cursor);

        SDL_free(appState);
    }
}
