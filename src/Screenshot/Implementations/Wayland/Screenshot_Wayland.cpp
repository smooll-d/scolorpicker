#include "Screenshot_Wayland.hpp"

#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <sdbus-c++/sdbus-c++.h>

#include <string>
#include <unistd.h>
#include <utility>
#include <cstdint>
#include <iostream>

namespace scp
{
    void Screenshot_Wayland::Take()
    {
        sdbus::ServiceName screenshotName{"org.freedesktop.portal.Desktop"};
        sdbus::ServiceName requestName{"org.freedesktop.portal.Request"};

        sdbus::ObjectPath screenshotPath{"/org/freedesktop/portal/desktop"};

        sdbus::InterfaceName screenshotInterface{"org.freedesktop.portal.Screenshot"};
        sdbus::InterfaceName requestInterface{"org.freedesktop.portal.Request"};

        std::string token{"scolorpicker_scp_screenshot"};

        try
        {
            _Connection = sdbus::createBusConnection();
            auto connectionName = _Connection->getUniqueName().erase(0, 1).replace(1, 1, "_");

            sdbus::ObjectPath screenshotRequestPath{std::format("/org/freedesktop/portal/desktop/request/{}/{}", connectionName, token)};

            auto requestProxy = sdbus::createProxy(*_Connection, screenshotName, std::move(screenshotRequestPath));
            requestProxy->uponSignal("Response")
                        .onInterface(requestInterface)
                        .call([this](uint32_t response, std::map<std::string, sdbus::Variant> results)
                                { this->_OnScreenshot(response, results); }
                        );

            auto screenshotProxy = sdbus::createProxy(*_Connection, screenshotName, std::move(screenshotPath));
            screenshotProxy->callMethod("Screenshot")
                            .onInterface(screenshotInterface)
                            .withArguments("dev.smooll.scolorpicker", std::map<std::string, sdbus::Variant>{
                                                                                      { "handle_token", sdbus::Variant(token) },
                                                                                      { "modal", sdbus::Variant(true) },
                                                                                      { "interactive", sdbus::Variant(false) }
                                                                                      });

            _Connection->enterEventLoop();
        }
        catch (const sdbus::Error &e)
        {
            std::cerr << e.getName() << ": " << e.getMessage() << '\n';
        }
    }

    SDL_Texture *Screenshot_Wayland::CreateTexture(SDL_Renderer *renderer)
    {
        SDL_Surface *surface = IMG_Load(this->_ScreenshotPath.c_str());
        if (!surface)
        {
            SDL_Log("Failed to create surface from screenshot: %s", SDL_GetError());

            SDL_Quit();
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            SDL_Log("Failed to create texture from surface: %s", SDL_GetError());

            SDL_Quit();
        }

        SDL_DestroySurface(surface);

        return texture;
    }

    void Screenshot_Wayland::_OnScreenshot(uint32_t response, std::map<std::string, sdbus::Variant> results)
    {
        _Connection->leaveEventLoop();

        if (response == 0)
        {
            this->_ScreenshotPath = results["uri"].get<std::string>().erase(0, 7);
        }
        else
        {
            std::cout << "Could not take screenshot!\n";
        }
    }
}
