#include "CLI/CLI.hpp"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "Screenshooter/Screenshooter.hpp"
#include "config.hpp"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <format>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <tuple>
#include <unistd.h>

// TODO: add clipboard support
// TODO: add color preview (rect and text)
// TODO: make a manpage
// TODO: README.md rewrite
// TODO: update AUR package
// TODO: create new AUR package for legacy (C) version
// TODO: merge scp2 into master
// TODO: profit

struct AppState
{
    std::filesystem::path cwd;

    scp::CLI cli;

    SDL_Window *window;

    SDL_Renderer *renderer;

    SDL_Texture *screenshot;

    SDL_Surface *screenshotPixels;

    SDL_Cursor *cursor;

    SDL_FRect colorView;
    SDL_FRect colorViewBorder;

    SDL_Color color;

    const SDL_PixelFormatDetails *pixelFormatDetails;

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

std::tuple<double, double, double> RGBToHSL(int red, int green, int blue)
{
    double r = std::clamp(static_cast<double>(red), 0.0, 255.0) / 255.0;
    double g = std::clamp(static_cast<double>(green), 0.0, 255.0) / 255.0;
    double b = std::clamp(static_cast<double>(blue), 0.0, 255.0) / 255.0;

    double cMin = std::min({r, g, b});
    double cMax = std::max({r, g, b});
    double delta = cMax - cMin;

    double h = 0.0;
    double l = (cMax + cMin) / 2.0;
    double s = (delta == 0) ? 0.0 : delta / (1.0 - std::abs(2.0 * l - 1.0));

    if (delta == 0)
        h = 0.0;
    else if (cMax == r)
        h = 60.0 * std::fmod((g - b) / delta, 6.0);
    else if (cMax == g)
        h = 60.0 * ((b - r) / delta + 2.0);
    else if (cMax == b)
        h = 60.0 * ((r - g) / delta + 4.0);

    if (h < 0.0)
        h += 360.0;

    return { h, s, l };
}

std::tuple<double, double, double> RGBToHSV(int red, int green, int blue)
{
    double r = std::clamp(static_cast<double>(red), 0.0, 255.0) / 255.0;
    double g = std::clamp(static_cast<double>(green), 0.0, 255.0) / 255.0;
    double b = std::clamp(static_cast<double>(blue), 0.0, 255.0) / 255.0;

    double cMin = std::min({r, g, b});
    double cMax = std::max({r, g, b});
    double delta = cMax - cMin;

    double h = 0.0;
    double v = cMax;
    double s = std::clamp(delta / cMax, 0.0, 1.0);

    if (delta == 0)
        h = 0.0;
    else if (cMax == 0)
        s = 0.0;
    else if (cMax == r)
        h = 60.0 * std::fmod((g - b) / delta, 6.0);
    else if (cMax == g)
        h = 60.0 * ((b - r) / delta + 2.0);
    else if (cMax == b)
        h = 60.0 * ((r - g) / delta + 4.0);

    if (h < 0.0)
        h += 360.0;

    return { h, s, v };
}

std::string HandleColor(AppState *appState, const std::string &color)
{
    if (appState->cli.GetInfo().output == "terminal")
        return color;
    else if (appState->cli.GetInfo().output == "clipboard")
    {
        if (!SDL_SetClipboardText(color.c_str()))
        {
            SDL_Log("Failed to copy color to clipboard: %s", SDL_GetError());

            SDL_Quit();
        }
    }

    return "Selected color copied to clipboard.";
}

std::string GetColor(AppState *appState, SDL_Surface *surface, int mouseX, int mouseY)
{
    SDL_Color color;

    const SDL_PixelFormatDetails *pixelFormatDetails = SDL_GetPixelFormatDetails(surface->format);

    uint32_t pixel = GetPixel(surface, mouseX, mouseY);

    SDL_GetRGB(pixel, pixelFormatDetails, nullptr, &color.r, &color.g, &color.b);

    pixel = color.r << 16 | color.g << 8 | color.b;

    if (appState->cli.GetInfo().format == "hex")
    {
        std::string colorString = std::format("#{:06X}", pixel);
        return HandleColor(appState, colorString);
    }
    else if (appState->cli.GetInfo().format == "lhex")
    {
        std::string colorString = std::format("#{:06x}", pixel);
        return HandleColor(appState, colorString);
    }
    else if (appState->cli.GetInfo().format == "rgb")
    {
        std::string colorString = std::format("rgb({}, {}, {})", color.r, color.g, color.b);
        return HandleColor(appState, colorString);
    }
    else if (appState->cli.GetInfo().format == "hsl")
    {
        auto hsl = RGBToHSL(color.r, color.g, color.b);

        std::string colorString = std::format("hsl({}, {}, {})",
                                              std::get<0>(hsl),
                                              std::get<1>(hsl) * 100.0,
                                              std::get<2>(hsl) * 100.0);

        return HandleColor(appState, colorString);
    }
    else if (appState->cli.GetInfo().format == "hsv")
    {
        auto hsv = RGBToHSL(color.r, color.g, color.b);

        std::string colorString = std::format("hsl({}, {}, {})",
                                              std::get<0>(hsv),
                                              std::get<1>(hsv) * 100.0,
                                              std::get<2>(hsv) * 100.0);

        return HandleColor(appState, colorString);
    }
    else if (appState->cli.GetInfo().format == "all")
    {
        auto hsl = RGBToHSL(color.r, color.g, color.b);
        auto hsv = RGBToHSV(color.r, color.g, color.b);

        std::string colorString = std::format(R"(#{0:06X}
#{0:06x}
rgb({1}, {2}, {3})
hsl({4:.1f}, {5:.1f}, {6:.1f})
hsv({7:.1f}, {8:.1f}, {9:.1f}))",
                                  pixel,
                                  color.r, color.g, color.b,
                                  std::get<0>(hsl), std::get<1>(hsl) * 100, std::get<2>(hsl) * 100,
                                  std::get<0>(hsv), std::get<1>(hsv) * 100, std::get<2>(hsv) * 100);

        return HandleColor(appState, colorString);
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

    appState->colorView.x = appState->mouseX + 10.0f;
    appState->colorView.y = appState->mouseY + 10.0f;
    appState->colorViewBorder.x = appState->colorView.x - 2.5f;
    appState->colorViewBorder.y = appState->colorView.y - 2.5f;

    if (appState->color.r >= 128 || appState->color.g >= 128 || appState->color.b >= 128)
        SDL_SetRenderDrawColor(appState->renderer, 0, 0, 0, 255);
    else if (appState->color.r <= 127 || appState->color.g <= 127 || appState->color.b <= 127)
        SDL_SetRenderDrawColor(appState->renderer, 255, 255, 255, 255);

    SDL_RenderFillRect(appState->renderer, &appState->colorViewBorder);

    if (!appState->screenshotPixels)
    {
        appState->screenshotPixels = SDL_RenderReadPixels(appState->renderer, nullptr);
        appState->pixelFormatDetails = SDL_GetPixelFormatDetails(appState->screenshotPixels->format);
    }

    uint32_t pixel = GetPixel(appState->screenshotPixels, appState->mouseX, appState->mouseY);

    SDL_GetRGB(pixel, appState->pixelFormatDetails, nullptr,
               &appState->color.r, &appState->color.g, &appState->color.b);

    SDL_SetRenderDrawColor(appState->renderer,
                           appState->color.r, appState->color.g, appState->color.b, appState->color.a);

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