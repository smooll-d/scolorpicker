#ifndef SCP_WINDOW
#define SCP_WINDOW

#include <SDL3/SDL.h>

namespace SCP
{
    class Window
    {
        public:
            Window();

            void Initialize
        private:
            SDL_Window* mWindow;
            SDL_Renderer* mRenderer;
    };
}

#endif // SCP_WINDOW
