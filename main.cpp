#include "AppState.hpp"
#include "CLI/CLI.hpp"
#include "Screenshooter/Screenshooter.hpp"
#include "Utils/Utils.hpp"
#include "config.hpp"
#include "Color/Color.hpp"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <format>
#include <iostream>
#include <unistd.h>

// TODO: add window icon
// TODO: flatpak package (?)
// TODO: profit

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    AppState *appState = static_cast<AppState*>(SDL_calloc(1, sizeof(AppState)));
    *appstate = appState;

    new (&appState->cli) scp::CLI{argc, argv};

    auto screenshooter{scp::Screenshooter::CreateInstance()};
    screenshooter->Take();

    SDL_SetAppMetadata("scolorpicker",
                       std::format("{}.{}.{}", SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH).c_str(),
                       "dev.smooll.scolorpicker");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::string SDLInitError = scp::Utils::Localize("main/sdl_init");

        SDL_Log("%s", scp::Utils::ReplacePlaceholder(SDLInitError, SDL_GetError()).c_str());

        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("scolorpicker",
                                     1920, 1080,
                                     SDL_WINDOW_FULLSCREEN,
                                     &appState->window,
                                     &appState->renderer))
    {
        std::string SDLWindowRendererError = scp::Utils::Localize("main/window_renderer_creation");

        SDL_Log("%s", scp::Utils::ReplacePlaceholder(SDLWindowRendererError, SDL_GetError()).c_str());

        return SDL_APP_FAILURE;
    }

    appState->CreateWindowIcon();

    SDL_DisplayID windowDisplayID = SDL_GetDisplayForWindow(appState->window);
    const SDL_DisplayMode *windowDisplay = SDL_GetDesktopDisplayMode(windowDisplayID);

    SDL_SetWindowSize(appState->window, windowDisplay->w, windowDisplay->h);

    appState->CreateCursor();

    appState->screenshot = screenshooter->CreateTexture(appState->renderer);

    appState->colorView.w = 100.0f;
    appState->colorView.h = 100.0f;
    appState->colorViewBorder.w = appState->colorView.w + 5.0f;
    appState->colorViewBorder.h = appState->colorView.h + 5.0f;

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
                default:
                    break;
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            switch (event->button.button)
            {
                case 1:
                {
                    SDL_Color color = scp::Color::Get(appState,
                                          appState->screenshotPixels,
                                          static_cast<int>(appState->mouseX),
                                          static_cast<int>(appState->mouseY));

                    std::cout << scp::Color::Format(appState, color);

                    return SDL_APP_SUCCESS;
                }
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
    AppState *appState = static_cast<AppState*>(appstate);

    SDL_GetMouseState(&appState->mouseX, &appState->mouseY);

    appState->colorView.x = appState->mouseX + 10.0f;
    appState->colorView.y = appState->mouseY + 10.0f;
    appState->colorViewBorder.x = appState->colorView.x - 2.5f;
    appState->colorViewBorder.y = appState->colorView.y - 2.5f;

    SDL_SetRenderDrawColor(appState->renderer, 0, 0, 0, 255);

    SDL_RenderClear(appState->renderer);

    SDL_RenderTexture(appState->renderer, appState->screenshot, nullptr, nullptr);

    if (!appState->screenshotPixels)
        appState->screenshotPixels = SDL_RenderReadPixels(appState->renderer, nullptr);

    SDL_Color color = scp::Color::Get(appState, appState->screenshotPixels, appState->mouseX, appState->mouseY);

    if (color.r >= 128 || color.g >= 128 || color.b >= 128)
        SDL_SetRenderDrawColor(appState->renderer, 0, 0, 0, 255);
    else if (color.r <= 127 || color.g <= 127 || color.b <= 127)
        SDL_SetRenderDrawColor(appState->renderer, 255, 255, 255, 255);

    SDL_RenderFillRect(appState->renderer, &appState->colorViewBorder);

    SDL_SetRenderDrawColor(appState->renderer,
                           color.r, color.g, color.b, color.a);

    SDL_RenderFillRect(appState->renderer, &appState->colorView);

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

        appState->cli.~CLI();

        SDL_free(appState);
    }
}