#include "CLI/CLI.hpp"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "Screenshooter/Screenshooter.hpp"
#include "config.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <format>
#include <iostream>
#include <filesystem>

struct AppState
{
    std::filesystem::path cwd;

    scp::CLI cli;

    SDL_Window *window;

    SDL_Renderer *renderer;

    SDL_Texture *screenshot;

    SDL_Surface *screenshotPixels;

    SDL_Cursor *cursor;

    float mouseX;
    float mouseY;

    int cursorW;
    int cursorH;

    void CreateCursor()
    {
        SDL_Surface *cursorSurfaces[4]
        {
            SDL_LoadBMP(std::format("{}data/cursors/cursor_16x16.bmp", cwd.c_str()).c_str()),
            SDL_LoadBMP(std::format("{}data/cursors/cursor_32x32.bmp", cwd.c_str()).c_str()),
            SDL_LoadBMP(std::format("{}data/cursors/cursor_64x64.bmp", cwd.c_str()).c_str()),
            SDL_LoadBMP(std::format("{}data/cursors/cursor_128x128.bmp", cwd.c_str()).c_str())
        };

        for (int i = 0; i < 4; i++)
        {
            if (!cursorSurfaces[i])
                SDL_Log("Failed to create cursor surface: %s", SDL_GetError());
        }

        for (int i = 1; i < 4; i++)
        {
            if (!SDL_AddSurfaceAlternateImage(cursorSurfaces[0], cursorSurfaces[i]))
                SDL_Log("Failed to add alternate cursor surface: %s", SDL_GetError());

            SDL_DestroySurface(cursorSurfaces[i]);
        }

        cursor = SDL_CreateColorCursor(cursorSurfaces[0], cursorSurfaces[0]->w / 2, cursorSurfaces[0]->h / 2);
        if (!cursor)
            SDL_Log("Failed to create cursor: %s", SDL_GetError());

        SDL_SetCursor(cursor);

        cursorW = cursorSurfaces[0]->w;
        cursorH = cursorSurfaces[0]->h;

        SDL_DestroySurface(cursorSurfaces[0]);
    }
};

uint32_t GetPixel(SDL_Surface *surface, int mouseX, int mouseY)
{
    const SDL_PixelFormatDetails *pixelFormatDetails = SDL_GetPixelFormatDetails(surface->format);

    uint8_t *pixel = static_cast<uint8_t*>(surface->pixels)
                   + mouseY * surface->pitch + mouseX * (pixelFormatDetails->bytes_per_pixel);

    switch (pixelFormatDetails->bytes_per_pixel)
    {
        case 1:
            return *pixel;
            break;
        case 2:
            return *reinterpret_cast<uint16_t*>(pixel);
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
            else
                return pixel[0] | pixel[1] << 8 | pixel[2] << 16;
            break;
        case 4:
            return *reinterpret_cast<uint32_t*>(pixel);
            break;
        default:
            return 0;
            break;
    }
}

std::string GetColor(AppState *appState, SDL_Surface *surface, int mouseX, int mouseY)
{
    SDL_Color color;

    const SDL_PixelFormatDetails *pixelFormatDetails = SDL_GetPixelFormatDetails(surface->format);

    uint32_t pixel = GetPixel(surface, mouseX, mouseY);

    SDL_GetRGB(pixel, pixelFormatDetails, nullptr, &color.r, &color.g, &color.b);

    pixel = color.r << 16 | color.g << 8 | color.b;

    if (appState->cli.GetInfo().format == "rgb")
        return std::format("rgb({},{},{})", color.r, color.b, color.g);
    else if (appState->cli.GetInfo().format == "hex")
        return std::format("#{:06X}", pixel);
    else if (appState->cli.GetInfo().format == "lhex")
        return std::format("#{:06x}", pixel);
    else if (appState->cli.GetInfo().format == "hsl")
    {
        double r = static_cast<double>(color.r) / 255.0;
        double g = static_cast<double>(color.g) / 255.0;
        double b = static_cast<double>(color.b) / 255.0;

        const double min = std::min({r, g, b});
        const double max = std::max({r, g, b});

        const double delta = max - min;

        double H = 0.0;
        double L = (max + min) / 2.0;
        double S = delta / (1.0 - std::abs(2.0 * L - 1.0));

        if (delta == 0.0)
        {
            H = 0.0;
            S = 0.0;
        }
        else if (max == r)
            H = 60.0 * std::fmod((g - b) / delta, 6.0);
        else if (max == g)
            H = 60.0 * ((b - r) / delta + 2.0);
        else if (max == b)
            H = 60.0 * ((r - g) / delta + 4.0);

        return std::format("hsl({},{},{})",
                           static_cast<int>(std::round(H)),
                           static_cast<int>(std::round(S * 100.0)),
                           static_cast<int>(std::round(L * 100.0)));
    }

    return "Failed to retrieve color!";
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    AppState *appState = static_cast<AppState*>(SDL_calloc(1, sizeof(AppState)));
    *appstate = appState;

    appState->cwd = argv[0];
    appState->cwd = appState->cwd.remove_filename();

    new (&appState->cli) scp::CLI{argc, argv};

    auto screenshooter{scp::Screenshooter::CreateInstance()};
    screenshooter->Take();

    SDL_SetAppMetadata("scolorpicker",
                       std::format("{}.{}.{}", SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH).c_str(),
                       "dev.smooll.scolorpicker");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("scolorpicker",
                                     1920, 1080,
                                     SDL_WINDOW_FULLSCREEN,
                                     &appState->window,
                                     &appState->renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    SDL_DisplayID windowDisplayID = SDL_GetDisplayForWindow(appState->window);
    const SDL_DisplayMode *windowDisplay = SDL_GetDesktopDisplayMode(windowDisplayID);

    SDL_SetWindowSize(appState->window, windowDisplay->w, windowDisplay->h);

    appState->CreateCursor();

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
                default:
                    break;
            }

            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            switch (event->button.button)
            {
                case 1:
                {
                    std::cout << GetColor(appState,
                                          appState->screenshotPixels,
                                          static_cast<int>(appState->mouseX),
                                          static_cast<int>(appState->mouseY)) << '\n';

                    return SDL_APP_SUCCESS;
                }
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
        appState->screenshotPixels = SDL_RenderReadPixels(appState->renderer, nullptr);

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