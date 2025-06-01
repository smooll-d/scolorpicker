#include "SCP_Utils.hpp"

#include <SDL3/SDL.h>

#include <sstream>

namespace SCP
{
    namespace Utils
    {
        std::string TRed(std::string_view str, std::string_view ground)
        {
            std::stringstream ss;

            if (ground == "background")
            { ss << "\e[41m" << str << "\e[0m"; }
            else if (ground == "foreground")
            { ss << "\e[1;31m" << str << "\e[0m"; }

            return ss.str();
        }

        void TestSDL(SDL_Renderer* renderer)
        {
            const double now = ((double)SDL_GetTicks()) / 1000.0;

            const float red = (float)(0.5 + 0.5 * SDL_sin(now));
            const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
            const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

            SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);
        }
    } // namespace Utils
} // namespace SCP
