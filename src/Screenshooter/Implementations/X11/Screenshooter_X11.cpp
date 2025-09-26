#include "Screenshooter_X11.hpp"
#include "Utils/Utils.hpp"

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
#include <SDL3/SDL_error.h>

#include <cstdlib>
#include <iostream>
#include <bit>

namespace scp
{
#ifdef SCP_ENABLE_XLIB
    void Screenshooter_X11::Take()
    {
        Display *display = XOpenDisplay(nullptr);
        if (!display)
        {
            std::cerr << Utils::Localize("Screenshooter/Screenshooter_X11/x_connection_init");

            std::exit(1);
        }

        Window root = DefaultRootWindow(display);
        if (!root)
        {
            std::cerr << Utils::Localize("Screenshooter/Screenshooter_X11/root_window");

            std::exit(1);
        }

        XWindowAttributes attributes {};
        if (!XGetWindowAttributes(display, root, &attributes))
        {
            std::cerr << Utils::Localize("Screenshooter/Screenshooter_X11/root_window_attributes");

            std::exit(1);
        }

        XImage *screenshot = XGetImage(display, root, 0, 0, attributes.width, attributes.height, AllPlanes, ZPixmap);
        if (!screenshot)
        {
            std::cerr << Utils::Localize("Screenshooter/Screenshooter_X11/screenshot_failure");

            std::exit(1);
        }

        this->_Info.width = screenshot->width;
        this->_Info.height = screenshot->height;
        this->_Info.pitch = screenshot->bytes_per_line;
        this->_Info.bitsPerPixel = screenshot->bits_per_pixel;
        this->_Info.redMask = screenshot->red_mask;
        this->_Info.greenMask = screenshot->green_mask;
        this->_Info.blueMask = screenshot->blue_mask;
        this->_Info.pixels = std::bit_cast<uint8_t*>(screenshot->data);
        this->_Info.size = screenshot->width * screenshot->height * (screenshot->bits_per_pixel / 8);

        this->ConvertPixelFormat();

        // NOTE: For some reason XDestroyImage() doesn't exist, so instead of using that, we'll just manually expand the macro
        screenshot->f.destroy_image(screenshot);

        XCloseDisplay(display);
    }
#endif // SCP_ENABLE_XLIB

#ifdef SCP_ENABLE_XCB
    void Screenshooter_X11::Take()
    {
        int defaultRootWindow;

        xcb_connection_t *connection = xcb_connect(nullptr, &defaultRootWindow);
        if (!connection)
        {
            std::cerr << Utils::Localize("Screenshooter/Screenshooter_X11/x_connection_init");

            std::exit(1);
        }

        xcb_screen_t *screen = _GetScreen(connection, defaultRootWindow);
        if (!screen)
        {
            std::cerr << Utils::Localize("Screenshooter/Screenshooter_X11/root_window");

            std::exit(1);
        }

        xcb_visualtype_t *visualType = _GetVisualType(connection, screen);
        if (!visualType)
        {
            std::cerr << Utils::Localize("Screenshooter/Screenshooter_X11/visual_type");

            std::exit(1);
        }

        /* NOTE: In Xlib, the plane_mask is usually set to AllPlanes. Internally that's just ~0UL, to get everything, all bits set.
         * xcb unfortunately doesn't have such a thing, it's really low level, but since in Xlib it's just ~0UL, we can set it here
         * as well. We do have to use ~0 static_cast to uint32_t instead of ~0UL as plane_mask is an unsigned 32 bit integer not an unsigned long. We could use ~0U, but I'd rather be safe than sorry.
         */
        xcb_image_t *screenshot = xcb_image_get(connection, screen->root, 0, 0, screen->width_in_pixels, screen->height_in_pixels, static_cast<uint32_t>(~0), XCB_IMAGE_FORMAT_Z_PIXMAP);
        if (!screenshot)
        {
            std::cerr << Utils::Localize("Screenshooter/Screenshooter_X11/screenshot_failure");

            std::exit(1);
        }

        this->_Info.width = screenshot->width;
        this->_Info.height = screenshot->height;
        this->_Info.pitch = screenshot->stride;
        this->_Info.bitsPerPixel = screenshot->bpp;
        this->_Info.redMask = visualType->red_mask;
        this->_Info.greenMask = visualType->green_mask;
        this->_Info.blueMask = visualType->blue_mask;
        this->_Info.pixels = std::bit_cast<uint8_t*>(screenshot->data);
        this->_Info.size = screenshot->size;

        this->ConvertPixelFormat();

        xcb_image_destroy(screenshot);

        xcb_disconnect(connection);
    }
#endif // SCP_ENABLE_XCB

    SDL_Texture *Screenshooter_X11::CreateTexture(SDL_Renderer *renderer)
    {
        SDL_Surface *surface = SDL_CreateSurfaceFrom(this->_Info.width,
                                                     this->_Info.height,
                                                     SDL_PIXELFORMAT_RGBA32,
                                                     this->_Info.pixels,
                                                     this->_Info.pitch);
        if (!surface)
        {
            std::string surfaceError = Utils::Localize("Screenshooter/surface_creation");

            SDL_Log("%s", Utils::ReplacePlaceholder(surfaceError, SDL_GetError()).c_str());

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

        this->_Destroy();

        return texture;
    }

    void Screenshooter_X11::ConvertPixelFormat()
    {
        int bytesPerPixel = this->_Info.bitsPerPixel / 8;

        uint8_t *convertedPixels = new uint8_t[this->_Info.size];

        std::size_t offset;

        uint32_t pixel;

        uint8_t red;
        uint8_t green;
        uint8_t blue;

        for (int x = 0; x < this->_Info.width; x++)
        {
            for (int y = 0; y < this->_Info.height; y++)
            {
                offset = y * this->_Info.pitch + x * bytesPerPixel;

                pixel = 0;

                /* NOTE: After a little bit of research, I've found that Wayland pixel formats are, by default,
                 * little endian, which means that we don't need to detect endianess there. On X11 pixel format
                 * endianess is dependent on the host system. That complicates things, but if I understand this
                 * correctly, SDL should take the endianess out of this by converting the pixel format itself.
                 * Currently we're doing SDL_PIXELFORMAT_RGBA32 which, under the hood, converts the format to either
                 * RGBA8888 or ABGR8888 for little and big endian respectively. I don't have a big endian system, so
                 * we're just going to have to wait and pray somebody with a big endian system doesn't make an issue on
                 * GitHub about this.
                 */
                for (int i = 0; i < bytesPerPixel; i++)
                {
                    pixel |= static_cast<uint32_t>(this->_Info.pixels[offset + i]) << (i * 8);
                }

                red = (pixel & this->_Info.redMask) >> scp::Utils::CountTrailingZeroes(this->_Info.redMask);
                green = (pixel & this->_Info.greenMask) >> scp::Utils::CountTrailingZeroes(this->_Info.greenMask);
                blue = (pixel & this->_Info.blueMask) >> scp::Utils::CountTrailingZeroes(this->_Info.blueMask);

                convertedPixels[offset] = red;
                convertedPixels[offset + 1] = green;
                convertedPixels[offset + 2] = blue;
                convertedPixels[offset + 3] = 255;
            }
        }

        this->_Info.pixels = convertedPixels;
    }

    void Screenshooter_X11::_Destroy()
    {
        delete[] this->_Info.pixels;
    }

#ifdef SCP_ENABLE_XCB
    xcb_screen_t *Screenshooter_X11::_GetScreen(xcb_connection_t *connection, int screen)
    {
        xcb_screen_iterator_t iterator;

        iterator = xcb_setup_roots_iterator(xcb_get_setup(connection));
        for (; iterator.rem; --screen, xcb_screen_next(&iterator))
        {
            if (screen == 0)
                return iterator.data;
        }

        return nullptr;
    }

    xcb_visualtype_t *Screenshooter_X11::_GetVisualType(xcb_connection_t *connection, xcb_screen_t *screen)
    {
        xcb_depth_iterator_t depthIterator = xcb_screen_allowed_depths_iterator(screen);
        for (; depthIterator.rem; xcb_depth_next(&depthIterator))
        {
            xcb_visualtype_iterator_t visualTypeIterator = xcb_depth_visuals_iterator(depthIterator.data);
            for (; visualTypeIterator.rem; xcb_visualtype_next(&visualTypeIterator))
            {
                if (screen->root_visual == visualTypeIterator.data->visual_id)
                    return visualTypeIterator.data;
            }
        }

        return nullptr;
    }
#endif // SCP_ENABLE_XCB
} // namespace scp