#ifndef SCP_SCREENSHOT
#define SCP_SCREENSHOT

#include <cstdint>
namespace scp
{
    class Screenshot
    {
        public:
            struct Image
            {
                int width;
                int height;
                int pitch;
                int bitsPerPixel;

                unsigned long redMask;
                unsigned long greenMask;
                unsigned long blueMask;

                std::uint8_t *pixels;
            };
    };
} // namespace scp

#endif // SCP_SCREENSHOT
