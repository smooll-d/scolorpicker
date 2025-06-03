#include "Screenshot_X11.hpp"

#include <X11/X.h>
#include <X11/Xlib.h>

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
        this->ConvertPixelFormat();

        // NOTE: For some reason XDestroyImage() doesn't exist, so instead of using that, we'll just manually expand the macro
        screenshot->f.destroy_image(screenshot);

        XCloseDisplay(display);
    }
} // namespace scp
