#ifndef SCP_COLOR
#define SCP_COLOR

#include "AppState.hpp"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>

#include <cstdint>
#include <string>
#include <tuple>

namespace scp
{
    namespace Color
    {
        uint32_t GetPixel(SDL_Surface *surface, int mouseX, int mouseY);

        SDL_Color Get(AppState *appState, SDL_Surface *surface, int mouseX, int mouseY);

        std::tuple<double, double, double> RGBToHSL(int red, int green, int blue);
        std::tuple<double, double, double> RGBToHSV(int red, int green, int blue);

        std::string Output(AppState *appState, const std::string &color);
        std::string Format(AppState *appState, const SDL_Color &color);
    } // namespace Color
} // namespace scp

#endif // SCP_COLOR