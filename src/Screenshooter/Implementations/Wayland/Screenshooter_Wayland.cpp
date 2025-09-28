#include "Screenshooter_Wayland.hpp"
#include "Utils/Utils.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

#include <sdbus-c++/sdbus-c++.h>

#include <stb_image/stb_image.h>

#include <string>
#include <unistd.h>
#include <cstdint>
#include <iostream>

namespace scp
{
    void Screenshooter_Wayland::Take()
    {
        sdbus::ServiceName desktopService{"org.freedesktop.portal.Desktop"};
        sdbus::ServiceName RequestService{"org.freedesktop.portal.Request"};

        sdbus::ObjectPath desktopObject{"/org/freedesktop/portal/desktop"};

        sdbus::InterfaceName screenshotInterface{"org.freedesktop.portal.Screenshot"};
        sdbus::InterfaceName requestInterface{"org.freedesktop.portal.Request"};

        sdbus::SignalName responseSignal{"Response"};

        sdbus::MethodName screenshotMethod{"Screenshot"};

        std::string token{"scolorpicker_scp_screenshot"};

        try
        {
            _Connection = sdbus::createBusConnection();
            auto connectionName = _Connection->getUniqueName().erase(0, 1).replace(1, 1, "_");

            sdbus::ObjectPath requestObject{std::format("/org/freedesktop/portal/desktop/request/{}/{}", connectionName, token)};

            auto requestProxy = sdbus::createProxy(*_Connection, desktopService, requestObject);
            requestProxy->uponSignal(responseSignal)
                         .onInterface(requestInterface)
                         .call([this](uint32_t response, std::map<std::string, sdbus::Variant> results)
                             { this->_OnScreenshot(response, results); }
                         );

            auto screenshotProxy = sdbus::createProxy(*_Connection, desktopService, desktopObject);
            screenshotProxy->callMethod(screenshotMethod)
                            .onInterface(screenshotInterface)
                            .withArguments("dev.smooll.scolorpicker", std::map<std::string, sdbus::Variant>
                            {
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

    SDL_Texture *Screenshooter_Wayland::CreateTexture(SDL_Renderer *renderer)
    {
        int width;
        int height;
        int bytesPerPixel;

        unsigned char *pixels = stbi_load(this->_ScreenshotPath.c_str(), &width, &height, &bytesPerPixel, 0);

        int pitch = width * bytesPerPixel;
        pitch = (pitch + 3) & ~3;

        int redMask;
        int greenMask;
        int blueMask;
        int alphaMask;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        redMask = 0x000000FF;
        greenMask = 0x0000FF00;
        blueMask = 0x00FF0000;
        alphaMask = (bytesPerPixel == 4) ? 0xFF000000 : 0;
#else
        int s = (bytesPerPixel == 4) ? 0 : 8;
        redMask = 0xFF000000 >> s;
        greenMask = 0x00FF0000 >> s;
        blueMask = 0x0000FF00 >> s;
        alphaMask = 0x000000FF >> s;
#endif

        SDL_PixelFormat pixelFormat = SDL_GetPixelFormatForMasks(bytesPerPixel * 8,
                                                                 redMask,
                                                                 greenMask,
                                                                 blueMask,
                                                                 alphaMask);

        SDL_Surface *surface = SDL_CreateSurfaceFrom(width, height, pixelFormat, pixels, pitch);
        if (!surface)
        {
            std::string surfaceError = Utils::Localize("Screenshooter/surface_creation");

            SDL_Log("%s", Utils::ReplacePlaceholder(surfaceError, SDL_GetError()).c_str());

            stbi_image_free(pixels);

            SDL_Quit();
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            std::string textureError = Utils::Localize("Screenshooter/texture_creation");

            SDL_Log("%s", Utils::ReplacePlaceholder(textureError, SDL_GetError()).c_str());

            SDL_Quit();
        }

        SDL_DestroySurface(surface);

        return texture;
    }

    void Screenshooter_Wayland::_OnScreenshot(uint32_t response, std::map<std::string, sdbus::Variant> results)
    {
        _Connection->leaveEventLoop();

        if (response == 0)
            this->_ScreenshotPath = results["uri"].get<std::string>().erase(0, 7);
        else
            std::cerr << Utils::Localize("Screenshooter/Screenshooter_Wayland/screenshot_failure.txt");
    }
}