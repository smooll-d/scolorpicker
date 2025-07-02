#ifndef SCP_SCREENSHOT_WAYLAND
#define SCP_SCREENSHOT_WAYLAND

#include "Screenshot/Screenshot.hpp"

#include "SDL3/SDL_render.h"

namespace scp
{
    class Screenshot_Wayland : public Screenshot
    {
        public:
            void Take() override;

            SDL_Texture *CreateTexture(SDL_Renderer *renderer) override;
    };
} // namespace scp

#endif // SCP_SCREENSHOT_WAYLAND
