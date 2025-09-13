#ifndef SCP_UTILS
#define SCP_UTILS

#include <SDL3/SDL.h>

#include <string>
#include <string_view>

namespace scp
{
    namespace Utils
    {
        std::string TRed(std::string_view str, std::string_view ground = "background");

        void TestSDL(SDL_Renderer *renderer);

        unsigned int CountTrailingZeroes(int number);

        int CheckSession();

        std::string Localize(std::string_view messageName);

        std::string ReplacePlaceholder(std::string placeholder, std::string_view actual);
    } // namespace Utils
} // namespace scp

#endif // SCP_UTILS