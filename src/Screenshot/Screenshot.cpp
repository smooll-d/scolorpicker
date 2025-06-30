#include "Screenshot.hpp"
#include "config.hpp"
#include "Utils/Utils.hpp"

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_render.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#ifdef SCP_ENABLE_X11
#include "Implementations/X11/Screenshot_X11.hpp"
#elif defined SCP_ENABLE_WAYLAND
#include "Implementations/Wayland/Screenshot_Wayland.hpp"
#elif defined SCP_ENABLE_LINUX
#include "Implementations/X11/Screenshot_X11.hpp"
#include "Implementations/Wayland/Screenshot_Wayland.hpp"
#endif // SCP_ENABLE_X11

namespace scp
{
    // Create an instance of Screenshot based on the platform.
    std::unique_ptr<Screenshot> Screenshot::CreateInstance()
    {
#ifdef SCP_ENABLE_X11
        return std::make_unique<Screenshot_X11>();
#elif defined SCP_ENABLE_WAYLAND
        return std::make_unique<Screenshot_Wayland>();
#elif defined SCP_ENABLE_LINUX
        if (std::getenv("DISPLAY"))
        {
            return std::make_unique<Screenshot_X11>();
        }
        else if (std::getenv("WAYLAND_DISPLAY"))
        {
            return std::make_unique<Screenshot_Wayland>();
        }
#endif

        std::cerr << "Failed to determine platform!\n";

        return nullptr;
    }

    // Converts an image's pixel data from it's format to RGBA.
    void Screenshot::ConvertPixelFormat()
    {
        int bytesPerPixel = this->_Info.bitsPerPixel / 8;

        uint8_t *convertedPixels = new uint8_t[this->_Info.size];

        for (int x = 0; x < this->_Info.width; x++)
        {
            for (int y = 0; y < this->_Info.height; y++)
            {
                std::size_t offset = y * this->_Info.pitch + x * bytesPerPixel;

                uint32_t pixel = 0;

                /* NOTE: After a little bit of research, I've found that Wayland pixel formats are, by default, little endian,
                 * which means that we don't need to detect endianess there. On X11 pixel format endianess is dependent on the host
                 * system. That complicates things, but if I understand this correctly, SDL should take the endianess out of this
                 * by converting the pixel format itself. Currently we're doing SDL_PIXELFORMAT_RGBA32 which, under the hood,
                 * converts the format to either RGBA8888 or ABGR8888 for little and big endian respectively.
                 * I don't have a big endian system, so we're just going to have to wait and pray somebody with a big endian system
                 * doesn't make an issue on GitHub about this.
                 */
                for (int i = 0; i < bytesPerPixel; i++)
                {
                    pixel |= static_cast<uint32_t>(this->_Info.pixels[offset + i]) << (i * 8);
                }

                uint8_t red = (pixel & this->_Info.redMask) >> scp::Utils::CountTrailingZeroes(this->_Info.redMask);
                uint8_t green = (pixel & this->_Info.greenMask) >> scp::Utils::CountTrailingZeroes(this->_Info.greenMask);
                uint8_t blue = (pixel & this->_Info.blueMask) >> scp::Utils::CountTrailingZeroes(this->_Info.blueMask);

                convertedPixels[offset] = red;
                convertedPixels[offset + 1] = green;
                convertedPixels[offset + 2] = blue;
                convertedPixels[offset + 3] = 255;
            }
        }

        this->_Info.pixels = convertedPixels;
    }

    // Destroys image pixel data
    void Screenshot::_Destroy()
    {
        delete[] this->_Info.pixels;
    }
} // namespace scp
