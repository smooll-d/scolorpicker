#ifndef SCP_MOUSE
#define SCP_MOUSE

#include "SDL3/SDL_mouse.h"

namespace SCP
{
    class Mouse
    {
        public:
            Mouse();

            void SetCursor();

            float GetPosition();

            SDL_MouseButtonFlags GetButton();

            float x;
            float y;
        private:
            SDL_Cursor* mCursor;

            SDL_MouseButtonFlags mButton;
    };
}

#endif // SCP_MOUSE
