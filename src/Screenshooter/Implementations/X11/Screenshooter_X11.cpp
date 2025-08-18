#include "Screenshooter_X11.hpp"

#ifdef SCP_ENABLE_XLIB
#include <X11/X.h>
#include <X11/Xlib.h>
#endif // SCP_ENABLE_XLIB

#ifdef SCP_ENABLE_XCB
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xproto.h>
#endif // SCP_ENABLE_XCB

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>

#include <cstdlib>
#include <iostream>

namespace scp
{

#ifdef SCP_ENABLE_XLIB
    // Populate Info struct with data using Xlib's XGetImage() function.
    void Screenshooter_X11::Take()
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
#endif // SCP_ENABLE_XLIB

#ifdef SCP_ENABLE_XCB
    // Populate Info struct with image data using xcb.
    void Screenshooter_X11::Take()
    {
        int defaultRootWindow;

        xcb_connection_t *connection = xcb_connect(nullptr, &defaultRootWindow);
        if (!connection)
        {
            std::cerr << "Failed to open connection to X server!\n";

            std::exit(1);
        }

        xcb_screen_t *screen = GetScreen(connection, defaultRootWindow);
        if (!screen)
        {
            std::cerr << "Failed to get root window!\n";

            std::exit(1);
        }

        xcb_visualtype_t *visualType = GetVisualType(connection, screen);
        if (!visualType)
        {
            std::cerr << "Failed to get the visual type of root!\n";

            std::exit(1);
        }

        /* NOTE: In Xlib, the plane_mask is usually set to AllPlanes. Internally that's just ~0UL, to get everything, all bits set.
         * xcb unfortunately doesn't have such a thing, it's really low level, but since in Xlib it's just ~0UL, we can set it here
         * as well. We do have to use ~0 static_cast to uint32_t instead of ~0UL as plane_mask is an unsigned 32 bit integer not an unsigned long. We could use ~0U, but I'd rather be safe than sorry.
         */
        xcb_image_t *screenshot = xcb_image_get(connection, screen->root, 0, 0, screen->width_in_pixels, screen->height_in_pixels, static_cast<uint32_t>(~0), XCB_IMAGE_FORMAT_Z_PIXMAP);
        if (!screenshot)
        {
            std::cerr << "Failed to take a screenshot!\n";

            std::exit(1);
        }

        this->SetWidth(screenshot->width);
        this->SetHeight(screenshot->height);
        this->SetPitch(screenshot->stride);
        this->SetBitsPerPixel(screenshot->bpp);
        this->SetRedMask(visualType->red_mask);
        this->SetGreenMask(visualType->green_mask);
        this->SetBlueMask(visualType->blue_mask);
        this->SetPixels(reinterpret_cast<uint8_t*>(screenshot->data));
        this->SetSize(screenshot->size);
        this->ConvertPixelFormat();

        xcb_image_destroy(screenshot);

        xcb_disconnect(connection);
    }
#endif // SCP_ENABLE_XCB

    SDL_Texture *Screenshooter_X11::CreateTexture(SDL_Renderer *renderer)
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

#ifdef SCP_ENABLE_XCB
    xcb_screen_t *Screenshooter_X11::GetScreen(xcb_connection_t *connection, int screen)
    {
        xcb_screen_iterator_t iterator;

        iterator = xcb_setup_roots_iterator(xcb_get_setup(connection));
        for (; iterator.rem; --screen, xcb_screen_next(&iterator))
        {
            if (screen == 0) {
                return iterator.data;
            }
        }

        return nullptr;
    }

    xcb_visualtype_t *Screenshooter_X11::GetVisualType(xcb_connection_t *connection, xcb_screen_t *screen)
    {
        xcb_depth_iterator_t depthIterator = xcb_screen_allowed_depths_iterator(screen);
        for (; depthIterator.rem; xcb_depth_next(&depthIterator))
        {
            xcb_visualtype_iterator_t visualTypeIterator = xcb_depth_visuals_iterator(depthIterator.data);
            for (; visualTypeIterator.rem; xcb_visualtype_next(&visualTypeIterator))
            {
                if (screen->root_visual == visualTypeIterator.data->visual_id)
                {
                    return visualTypeIterator.data;
                }
            }
        }

        return nullptr;
    }
#endif // SCP_ENABLE_XCB
} // namespace scp
