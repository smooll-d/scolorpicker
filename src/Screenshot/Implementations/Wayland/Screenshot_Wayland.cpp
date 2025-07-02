#include "Screenshot_Wayland.hpp"

#include "SDL3/SDL_render.h"

#include <sdbus-c++/IConnection.h>

namespace scp
{
    void Screenshot_Wayland::Take()
    {
        return;
    }

    SDL_Texture *Screenshot_Wayland::CreateTexture(SDL_Renderer *renderer)
    {
        return nullptr;
    }
}
