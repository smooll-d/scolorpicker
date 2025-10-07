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

    appState->window = SDL_CreateWindow("scolorpicker", 640, 480, 0);
    if (!appState->window)
    {
        std::string SDLWindowRendererError = scp::Utils::Localize("main/window_creation");

        SDL_Log("%s", scp::Utils::ReplacePlaceholder(SDLWindowRendererError, SDL_GetError()).c_str());

        return SDL_APP_FAILURE;
    }

    appState->CreateWindowIcon();
    appState->CreateCursor();

    SDL_DisplayID windowDisplayID = SDL_GetDisplayForWindow(appState->window);
    const SDL_DisplayMode *windowDisplayMode = SDL_GetDesktopDisplayMode(windowDisplayID);

    appState->windowWidth = windowDisplayMode->w;
    appState->windowHeight = windowDisplayMode->h;

    SDL_SetWindowSize(appState->window, windowDisplayMode->w, windowDisplayMode->h);
    SDL_SetWindowFullscreen(appState->window, true);

    appState->renderer = SDL_CreateRenderer(appState->window, nullptr);
    if (!appState->renderer)
    {
        std::string rendererCreationError = scp::Utils::Localize("main/renderer_creation");

        SDL_Log("%s", scp::Utils::ReplacePlaceholder(rendererCreationError, SDL_GetError()).c_str());

        return SDL_APP_FAILURE;
    }

    appState->screenshot = screenshooter->CreateTexture(appState->renderer);

    appState->borderThickness = 3.0f;

    appState->colorView.radius = 15.0f;
    appState->colorView.width = 100.0f;
    appState->colorView.height = 100.0f;

    appState->colorViewBorder.radius = appState->colorView.radius + appState->borderThickness;
    appState->colorViewBorder.width = appState->colorView.width + 2.0f * appState->borderThickness;
    appState->colorViewBorder.height = appState->colorView.height + 2.0f * appState->borderThickness;

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
    appState->colorViewBorder.x = appState->colorView.x - appState->borderThickness;
    appState->colorViewBorder.y = appState->colorView.y - appState->borderThickness;

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

    if (appState->colorViewBorder.width + appState->colorViewBorder.x >= appState->windowWidth)
    {
        appState->colorView.x -= appState->colorViewBorder.width + appState->borderThickness + 10.0f;
        appState->colorViewBorder.x -= appState->colorViewBorder.height + appState->borderThickness + 10.0f;
    }
    if (appState->colorViewBorder.height + appState->colorViewBorder.y >= appState->windowHeight)
    {
        appState->colorView.y -= appState->colorViewBorder.width + appState->borderThickness + 10.0f;
        appState->colorViewBorder.y -= appState->colorViewBorder.height + appState->borderThickness + 10.0f;
    }

    appState->colorViewBorder.Draw(appState->renderer);

    SDL_SetRenderDrawColor(appState->renderer,
                           color.r, color.g, color.b, color.a);

    appState->colorView.Draw(appState->renderer);

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