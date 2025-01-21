#include "SDL3/SDL_mouse.h"
#include <SCP/SCP_Mouse.hpp>

#include <SDL3/SDL.h>

namespace SCP
{
    Mouse::Mouse()
     : mCursor{SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR)},
       mButton{}
    {
        if (!mCursor)
        { SDL_Log("Failed to create system cursor: %s", SDL_GetError()); }
    }

    void Mouse::SetCursor()
    { SDL_SetCursor(mCursor); }

    float Mouse::GetPosition()
    { return SDL_GetGlobalMouseState(&x, &y); }

    SDL_MouseButtonFlags Mouse::GetButton()
    {
        mButton = SDL_BUTTON_MASK(SDL_GetGlobalMouseState(&x, &y));

        return mButton;
    }
}
