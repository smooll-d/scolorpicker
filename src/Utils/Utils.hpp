#ifndef SCP_UTILS
#define SCP_UTILS

#include <SDL3/SDL.h>

#include <string>
#include <string_view>
#include <filesystem>

namespace scp
{
    namespace Utils
    {
        std::string TRed(std::string_view str, std::string_view ground = "background");

        void TestSDL(SDL_Renderer *renderer);
        void DrawFilledCircle(SDL_Renderer *renderer, std::pair<float, float> center, float radius);
        void DrawFilledRoundedRectangle(SDL_Renderer *renderer,
                                        std::pair<float, float> position,
                                        std::pair<float, float> size,
                                        float radius);

        unsigned int CountTrailingZeroes(int number);

        int CheckSession();

        double DegreesToRadians(int degrees);

        std::string Localize(std::string_view messageName);

        std::string ReplacePlaceholder(std::string placeholder, std::string_view actual);

        SDL_Surface *CreateSurfaceFromSTBI(std::filesystem::path filepath);
    } // namespace Utils
} // namespace scp

#endif // SCP_UTILS