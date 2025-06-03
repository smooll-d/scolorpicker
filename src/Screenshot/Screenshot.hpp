#ifndef SCP_SCREENSHOT
#define SCP_SCREENSHOT

#include <cstdint>

namespace scp
{
    class Image
    {
        public:
            struct Info
            {
                int width;
                int height;
                int pitch;
                int bitsPerPixel;

                unsigned long redMask;
                unsigned long greenMask;
                unsigned long blueMask;

                uint8_t *pixels;
            };
    };
} // namespace scp

#endif // SCP_SCREENSHOT
