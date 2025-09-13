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

struct AppState
{
    scp::CLI cli;

    SDL_Window *window;

    SDL_Renderer *renderer;

    SDL_Texture *screenshot;

    SDL_Surface *screenshotPixels;

    SDL_Cursor *cursor;

    SDL_FRect colorView;
    SDL_FRect colorViewBorder;

    float mouseX;
    float mouseY;

    int cursorW;
    int cursorH;

    void CreateCursor()
    {
        SDL_Surface *cursorSurfaces[4]
        {
            SDL_LoadBMP(std::format("{}/cursor/cursor_16x16.bmp", cli.cwd.c_str()).c_str()),
            SDL_LoadBMP(std::format("{}/cursor/cursor_32x32.bmp", cli.cwd.c_str()).c_str()),
            SDL_LoadBMP(std::format("{}/cursor/cursor_64x64.bmp", cli.cwd.c_str()).c_str()),
            SDL_LoadBMP(std::format("{}/cursor/cursor_128x128.bmp", cli.cwd.c_str()).c_str())
        };

        for (int i = 0; i < 4; i++)
        {
            if (!cursorSurfaces[i])
            {
                std::string cursorSurfaceError = scp::Utils::Localize("AppState/cursor_surface");

                SDL_Log("%s", scp::Utils::ReplacePlaceholder(cursorSurfaceError, SDL_GetError()).c_str());
            }
        }

        for (int i = 1; i < 4; i++)
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

        SDL_SetCursor(cursor);

        cursorW = cursorSurfaces[0]->w;
        cursorH = cursorSurfaces[0]->h;

        SDL_DestroySurface(cursorSurfaces[0]);
    }
};

#endif // SCP_APPSTATE