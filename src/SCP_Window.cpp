#include "SDL3/SDL_init.h"
#include <SCP/SCP_Window.hpp>
#include <SCP/metadata.hpp>

#include <format>

namespace SCP
{
    Window::Window()
     : mWindow{nullptr},
       mRenderer{nullptr}
    { }

    SDL_AppResult Window::Initialize()
    {
        SDL_SetAppMetadata("scolorpicker", std::format("{}.{}.{}", SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH).c_str(), "com.smooll.scolorpicker");

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("Failed to initialize SDL: %s", SDL_GetError());

            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    SDL_AppResult Window::Create()
    {
        if (!SDL_CreateWindowAndRenderer("scolorpicker", 100, 100, 0, &mWindow, &mRenderer))
        {
            SDL_Log("Failed to create window and renderer: %s", SDL_GetError());

            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    SDL_AppResult Window::Loop()
    {
        const double now = ((double)SDL_GetTicks()) / 1000.0;

        const float red = (float)(0.5 + 0.5 * SDL_sin(now));
        const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
        const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

        SDL_SetRenderDrawColorFloat(mRenderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);

        SDL_RenderClear(mRenderer);

        SDL_RenderPresent(mRenderer);

        return SDL_APP_CONTINUE;
    }
}
