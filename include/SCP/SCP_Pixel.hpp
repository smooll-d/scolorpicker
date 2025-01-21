#ifndef SCP_PIXEL
#define SCP_PIXEL

#include <vector>

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif // __linux__

namespace SCP
{
#ifdef __linux__
    class Pixel
    {
        public:
            Pixel();
            ~Pixel();

            std::vector<unsigned short> GetPixel(int x, int y);

            std::vector<unsigned short> pixels;
        private:
            XColor mColor;

            Display* mDisplay;

            XImage* mImage;
    };
#endif // __linux__
}

#endif // SCP_PIXEL
