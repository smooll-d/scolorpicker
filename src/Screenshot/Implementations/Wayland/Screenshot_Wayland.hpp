#ifndef SCP_SCREENSHOT_WAYLAND
#define SCP_SCREENSHOT_WAYLAND

#include "Screenshot/Screenshot.hpp"
#include "sdbus-c++/IProxy.h"
#include "sdbus-c++/Message.h"

#include <SDL3/SDL_render.h>
#include <sdbus-c++/sdbus-c++.h>

#include <memory>
#include <string>
#include <cstdint>

namespace scp
{
    class Screenshot_Wayland : public Screenshot
    {
        public:
            void Take() override;

            SDL_Texture *CreateTexture(SDL_Renderer *renderer) override;
        private:
            void _OnScreenshot(uint32_t response, std::map<std::string, sdbus::Variant> results);

            std::string _ScreenshotPath;

            std::unique_ptr<sdbus::IConnection> _Connection;
    };
} // namespace scp

#endif // SCP_SCREENSHOT_WAYLAND
