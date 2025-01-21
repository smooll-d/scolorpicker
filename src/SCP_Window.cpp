#include <SCP/SCP_Window.hpp>
#include <SCP/SCP_Utils.hpp>

#include <SCP/metadata.hpp>

#include <format>

namespace SCP
{
    Window::Window()
     : mWindow{nullptr},
       mRenderer{nullptr},
       mDisplayID{},
       mCurrentDisplay{nullptr},
       mMouse{},
       mPositionOffset{10.0f},
       mX{0},
       mY{0},
       mW{100},
       mH{100},
       mPixels{},
       mPixel{}
    { }

    SDL_AppResult Window::Initialize()
    {
        SDL_SetAppMetadata("scolorpicker", std::format("{}.{}.{}", SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH).c_str(), "com.smooll.scolorpicker");

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("Failed to initialize SDL: %s", SDL_GetError());

            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    SDL_AppResult Window::Create()
    {
        if (!SDL_CreateWindowAndRenderer("scolorpicker", 100, 100, 0, &mWindow, &mRenderer))
        {
            SDL_Log("Failed to create window and renderer: %s", SDL_GetError());

            return SDL_APP_FAILURE;
        }

        mMouse.SetCursor();

        mDisplayID = SDL_GetDisplayForWindow(mWindow);
        mCurrentDisplay = SDL_GetDesktopDisplayMode(mDisplayID);

        return SDL_APP_CONTINUE;
    }

    SDL_AppResult Window::Iterate()
    {
        mMouse.GetPosition();
        SDL_GetWindowPosition(mWindow, &mX, &mY);

        mPixel.GetPixel(mMouse.x, mMouse.y);
        mPixels = Utils::RGB(mPixel.pixels);

        if (mMouse.GetButton() == SDL_BUTTON_LEFT)
        { SDL_Log("X: %f, Y: %f", mMouse.x, mMouse.y); }

        if (mX + mW >= mCurrentDisplay->w)
        { SDL_SetWindowPosition(mWindow, (mMouse.x + mPositionOffset) - mW, mMouse.y + mPositionOffset); }
        else if (mY + mH >= mCurrentDisplay->h)
        { SDL_SetWindowPosition(mWindow, mMouse.x + mPositionOffset, (mMouse.y + mPositionOffset) - mH); }
        else
        { SDL_SetWindowPosition(mWindow, mMouse.x + mPositionOffset, mMouse.y + mPositionOffset); }

        SDL_SyncWindow(mWindow);

        SDL_SetRenderDrawColor(mRenderer, mPixels.at(0), mPixels.at(1), mPixels.at(2), SDL_ALPHA_OPAQUE_FLOAT);
        SDL_RenderClear(mRenderer);
        SDL_RenderPresent(mRenderer);

        return SDL_APP_CONTINUE;
    }
}
