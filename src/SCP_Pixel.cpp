#include <SCP/SCP_Pixel.hpp>

#include <vector>

namespace SCP
{
#ifdef __linux__
    Pixel::Pixel()
     : pixels{},
       mColor{},
       mDisplay{nullptr},
       mImage{nullptr}
    { mDisplay = XOpenDisplay((char*)NULL); }

    Pixel::~Pixel()
    { XFree(mImage); }

    std::vector<unsigned short> Pixel::GetPixel(int x, int y)
    {
        mImage = XGetImage(mDisplay, XRootWindow(mDisplay, XDefaultScreen(mDisplay)), x, y, 1, 1, AllPlanes, XYPixmap);

        mColor.pixel = XGetPixel(mImage, 0, 0);

        XQueryColor(mDisplay, XDefaultColormap(mDisplay, XDefaultScreen(mDisplay)), &mColor);

        return pixels = { mColor.red, mColor.green, mColor.blue };
    }
#endif // __linux__
}
