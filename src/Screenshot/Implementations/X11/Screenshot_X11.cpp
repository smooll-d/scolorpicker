#include "Screenshot_X11.hpp"

#include <X11/X.h>
#include <X11/Xlib.h>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>

#include <cstdlib>
#include <iostream>

namespace scp
{
    // Populate Info struct with data using Xlib's XGetImage() function.
    void Screenshot_X11::Take()
    {
        Display *display = XOpenDisplay(nullptr);
        if (!display)
        {
            std::cerr << "Failed to open connection to X server!\n";

            std::exit(1);
        }

        Window root = DefaultRootWindow(display);
        if (!root)
        {
            std::cerr << "Failed to get root window!\n";

            std::exit(1);
        }

        XWindowAttributes attributes {};
        if (!XGetWindowAttributes(display, root, &attributes))
        {
            std::cerr << "Failed to get root window attributes!\n";

            std::exit(1);
        }

        XImage *screenshot = XGetImage(display, root, 0, 0, attributes.width, attributes.height, AllPlanes, ZPixmap);
        if (!screenshot)
        {
            std::cerr << "Failed to take a screenshot!\n";

            std::exit(1);
        }

        this->SetWidth(screenshot->width);
        this->SetHeight(screenshot->height);
        this->SetPitch(screenshot->bytes_per_line);
        this->SetBitsPerPixel(screenshot->bits_per_pixel);
        this->SetRedMask(screenshot->red_mask);
        this->SetGreenMask(screenshot->green_mask);
        this->SetBlueMask(screenshot->blue_mask);
        this->SetPixels(reinterpret_cast<uint8_t*>(screenshot->data));
        this->SetSize(screenshot->width * screenshot->height * (screenshot->bits_per_pixel / 8));
        this->ConvertPixelFormat();

        // NOTE: For some reason XDestroyImage() doesn't exist, so instead of using that, we'll just manually expand the macro
        screenshot->f.destroy_image(screenshot);

        XCloseDisplay(display);
    }

    SDL_Texture *Screenshot_X11::CreateTexture(SDL_Renderer *renderer)
    {
        SDL_Surface *surface = SDL_CreateSurfaceFrom(this->_Info.width, this->_Info.height, SDL_PIXELFORMAT_RGBA32, this->_Info.pixels, this->_Info.pitch);
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

        this->_Destroy();

        return texture;
    }
} // namespace scp
