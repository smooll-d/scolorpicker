#ifndef SCP_APPSTATE
#define SCP_APPSTATE

#include "CLI/CLI.hpp"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>

#include <filesystem>
#include <format>

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

    float mouseX;
    float mouseY;

    int cursorW;
    int cursorH;

    void CreateCursor()
    {
        SDL_Surface *cursorSurfaces[4]
        {
            SDL_LoadBMP(std::format("{}/cursor/cursor_16x16.bmp", cwd.c_str()).c_str()),
            SDL_LoadBMP(std::format("{}/cursor/cursor_32x32.bmp", cwd.c_str()).c_str()),
            SDL_LoadBMP(std::format("{}/cursor/cursor_64x64.bmp", cwd.c_str()).c_str()),
            SDL_LoadBMP(std::format("{}/cursor/cursor_128x128.bmp", cwd.c_str()).c_str())
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

#endif // SCP_APPSTATE