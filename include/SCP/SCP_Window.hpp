#ifndef SCP_WINDOW
#define SCP_WINDOW

#include <SCP/SCP_Pixel.hpp>
#include <SCP/SCP_Mouse.hpp>

#include <SDL3/SDL.h>

namespace SCP
{
    class Window
    {
        public:
            Window();

            SDL_AppResult Initialize();
            SDL_AppResult Create();
            SDL_AppResult Iterate();
        private:
            SDL_Window* mWindow;

            SDL_Renderer* mRenderer;

            SDL_DisplayID mDisplayID;
            const SDL_DisplayMode* mCurrentDisplay;

            Mouse mMouse;

            float mPositionOffset;

            int mX;
            int mY;
            int mW;
            int mH;

            std::vector<int> mPixels;

            Pixel mPixel;
    };
}

#endif // SCP_WINDOW
