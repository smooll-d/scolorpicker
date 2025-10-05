#ifndef SCP_APPSTATE
#define SCP_APPSTATE

#include "CLI/CLI.hpp"
#include "Utils/Utils.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>

#include <format>

struct ColorView
{
    float x;
    float y;
    float width;
    float height;
    float radius;

    void Draw(SDL_Renderer *renderer)
    {
        scp::Utils::DrawFilledRoundedRectangle(renderer, { x, y }, { width, height }, radius);
    }
};

struct AppState
{
    scp::CLI cli;

    SDL_Window *window;

    int windowWidth;
    int windowHeight;

    SDL_Renderer *renderer;

    SDL_Texture *screenshot;

    SDL_Surface *screenshotPixels;

    SDL_Cursor *cursor;

    ColorView colorView;
    ColorView colorViewBorder;

    float mouseX;
    float mouseY;
    float borderThickness;

    void CreateCursor()
    {
        SDL_Surface *cursorSurfaces[2]
        {
            SDL_LoadBMP(std::format("{}/cursor/cursor_32x32.bmp", cli.cwd.c_str()).c_str()),
            SDL_LoadBMP(std::format("{}/cursor/cursor_16x16.bmp", cli.cwd.c_str()).c_str()),
        };

        for (int i = 0; i < 2; i++)
        {
            if (!cursorSurfaces[i])
            {
                std::string cursorSurfaceError = scp::Utils::Localize("AppState/cursor_surface");

                SDL_Log("%s", scp::Utils::ReplacePlaceholder(cursorSurfaceError, SDL_GetError()).c_str());
            }
        }

        for (int i = 1; i < 2; i++)
        {
            if (!SDL_AddSurfaceAlternateImage(cursorSurfaces[0], cursorSurfaces[i]))
            {
                std::string alternateError = scp::Utils::Localize("AppState/alternate_cursors");

                SDL_Log("%s", scp::Utils::ReplacePlaceholder(alternateError, SDL_GetError()).c_str());
            }

            SDL_DestroySurface(cursorSurfaces[i]);
        }

        cursor = SDL_CreateColorCursor(cursorSurfaces[0], cursorSurfaces[0]->w / 2, cursorSurfaces[0]->h / 2);
        if (!cursor)
        {
            std::string cursorError = scp::Utils::Localize("AppState/cursor_creation");

            SDL_Log("%s", scp::Utils::ReplacePlaceholder(cursorError, SDL_GetError()).c_str());
        }

        if (!SDL_SetCursor(cursor))
        {
            std::string setCursorError = scp::Utils::Localize("AppState/set_cursor");

            SDL_Log("%s", scp::Utils::ReplacePlaceholder(setCursorError, SDL_GetError()).c_str());
        }

        SDL_DestroySurface(cursorSurfaces[0]);
    }

    void CreateWindowIcon()
    {
        SDL_Surface *iconSurfaces[4]
        {
            scp::Utils::CreateSurfaceFromSTBI(std::format("{}/logo/logo_16x16.png", cli.cwd)),
            scp::Utils::CreateSurfaceFromSTBI(std::format("{}/logo/logo_32x32.png", cli.cwd)),
            scp::Utils::CreateSurfaceFromSTBI(std::format("{}/logo/logo_64x64.png", cli.cwd)),
            scp::Utils::CreateSurfaceFromSTBI(std::format("{}/logo/logo_128x128.png", cli.cwd))
        };

        for (int i = 0; i < 4; i++)
        {
            if (!iconSurfaces[i])
            {
                std::string iconSurfaceError = scp::Utils::Localize("AppState/icon_surface");

                SDL_Log("%s", scp::Utils::ReplacePlaceholder(iconSurfaceError, SDL_GetError()).c_str());
            }
        }

        for (int i = 1; i < 4; i++)
        {
            if (!SDL_AddSurfaceAlternateImage(iconSurfaces[0], iconSurfaces[i]))
            {
                std::string alternateIconError = scp::Utils::Localize("AppState/alternate_icons");

                SDL_Log("%s", scp::Utils::ReplacePlaceholder(alternateIconError, SDL_GetError()).c_str());
            }

            SDL_DestroySurface(iconSurfaces[i]);
        }

        if (!SDL_SetWindowIcon(window, iconSurfaces[0]))
        {
            std::string windowIconError = scp::Utils::Localize("AppState/window_icon");

            SDL_Log("%s", scp::Utils::ReplacePlaceholder(windowIconError, SDL_GetError()).c_str());
        }

        SDL_DestroySurface(iconSurfaces[0]);
    }
};

#endif // SCP_APPSTATE