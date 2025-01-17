#ifndef SCP_WINDOW
#define SCP_WINDOW

#include "SDL3/SDL_init.h"
#include <SDL3/SDL.h>

namespace SCP
{
    class Window
    {
        public:
            Window();

            SDL_AppResult Initialize();
            SDL_AppResult Create();
            SDL_AppResult Loop();
        private:
            SDL_Window* mWindow;

            SDL_Renderer* mRenderer;
    };
}

#endif // SCP_WINDOW
