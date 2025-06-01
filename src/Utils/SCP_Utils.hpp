#ifndef SCP_UTILS
#define SCP_UTILS

#include <SDL3/SDL.h>

#include <string>

namespace SCP
{
    namespace Utils
    {
        std::string TRed(std::string_view str, std::string_view ground = "background");
        void TestSDL(SDL_Renderer* renderer);
    } // namespace Utils
} // namespace SCP

#endif // SCP_UTILS
