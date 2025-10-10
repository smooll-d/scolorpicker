#include "Color.hpp"
#include "Utils/Utils.hpp"

#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

namespace scp
{
    uint32_t Color::GetPixel(SDL_Surface *surface, int mouseX, int mouseY)
    {
        const SDL_PixelFormatDetails *pixelFormatDetails = SDL_GetPixelFormatDetails(surface->format);

        uint8_t *pixel = static_cast<uint8_t*>(surface->pixels)
                       + mouseY * surface->pitch + mouseX * (pixelFormatDetails->bytes_per_pixel);

        switch (pixelFormatDetails->bytes_per_pixel)
        {
            case 1:
                return *pixel;
                break;
            case 2:
                return *reinterpret_cast<uint16_t*>(pixel);
                break;
            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
                else
                    return pixel[0] | pixel[1] << 8 | pixel[2] << 16;
                break;
            case 4:
                return *reinterpret_cast<uint32_t*>(pixel);
                break;
            default:
                return 0;
                break;
        }
    }

    SDL_Color Color::Get(AppState *appState, SDL_Surface *surface, int mouseX, int mouseY)
    {
        SDL_Color color;

        const SDL_PixelFormatDetails *pixelFormatDetails = SDL_GetPixelFormatDetails(surface->format);

        uint32_t pixel = GetPixel(surface, mouseX, mouseY);

        SDL_GetRGB(pixel, pixelFormatDetails, nullptr, &color.r, &color.g, &color.b);

        return color;
    }

    std::tuple<double, double, double> Color::RGBToHSL(int red, int green, int blue)
    {
        double r = std::clamp(static_cast<double>(red), 0.0, 255.0) / 255.0;
        double g = std::clamp(static_cast<double>(green), 0.0, 255.0) / 255.0;
        double b = std::clamp(static_cast<double>(blue), 0.0, 255.0) / 255.0;

        double cMin = std::min({r, g, b});
        double cMax = std::max({r, g, b});
        double delta = cMax - cMin;

        double h = 0.0;
        double l = (cMax + cMin) / 2.0;
        double s = (delta == 0) ? 0.0 : delta / (1.0 - std::abs(2.0 * l - 1.0));

        if (delta == 0)
            h = 0.0;
        else if (cMax == r)
            h = 60.0 * std::fmod((g - b) / delta, 6.0);
        else if (cMax == g)
            h = 60.0 * ((b - r) / delta + 2.0);
        else if (cMax == b)
            h = 60.0 * ((r - g) / delta + 4.0);

        if (h < 0.0)
            h += 360.0;

        return { h, s, l };
    }

    std::tuple<double, double, double> Color::RGBToHSV(int red, int green, int blue)
    {
        double r = std::clamp(static_cast<double>(red), 0.0, 255.0) / 255.0;
        double g = std::clamp(static_cast<double>(green), 0.0, 255.0) / 255.0;
        double b = std::clamp(static_cast<double>(blue), 0.0, 255.0) / 255.0;

        double cMin = std::min({r, g, b});
        double cMax = std::max({r, g, b});
        double delta = cMax - cMin;

        double h = 0.0;
        double v = cMax;
        double s = std::clamp(delta / cMax, 0.0, 1.0);

        if (delta == 0)
            h = 0.0;
        else if (cMax == 0)
            s = 0.0;
        else if (cMax == r)
            h = 60.0 * std::fmod((g - b) / delta, 6.0);
        else if (cMax == g)
            h = 60.0 * ((b - r) / delta + 2.0);
        else if (cMax == b)
            h = 60.0 * ((r - g) / delta + 4.0);

        if (h < 0.0)
            h += 360.0;

        return { h, s, v };
    }

    std::string Color::Output(AppState *appState, const std::string &color)
    {
        if (appState->cli.GetInfo().output == "terminal")
            return color + '\n';
        else if (appState->cli.GetInfo().output == "clipboard")
        {
            bool inClipboard = SDL_SetClipboardText(color.c_str());
            if (!inClipboard)
            {
                std::string notInClipboardError = Utils::Localize("Color/not_in_clipboard");

                SDL_Log("%s", Utils::ReplacePlaceholder(notInClipboardError, SDL_GetError()).c_str());

                SDL_Quit();
            }
        }

        return Utils::Localize("Color/copied_to_clipboard");
    }

    std::string Color::Format(AppState *appState, const SDL_Color &color)
    {
        uint32_t pixel = color.r << 16 | color.g << 8 | color.b;

        if (appState->cli.GetInfo().format == "hex")
        {
            std::string colorString = std::format("#{:06X}", pixel);
            return Output(appState, colorString);
        }
        else if (appState->cli.GetInfo().format == "lhex")
        {
            std::string colorString = std::format("#{:06x}", pixel);
            return Output(appState, colorString);
        }
        else if (appState->cli.GetInfo().format == "rgb")
        {
            std::string colorString = std::format("rgb({}, {}, {})", color.r, color.g, color.b);
            return Output(appState, colorString);
        }
        else if (appState->cli.GetInfo().format == "hsl")
        {
            auto hsl = RGBToHSL(color.r, color.g, color.b);

            std::string colorString = std::format("hsl({}, {}, {})",
                                                  std::get<0>(hsl),
                                                  std::get<1>(hsl) * 100.0,
                                                  std::get<2>(hsl) * 100.0);

            return Output(appState, colorString);
        }
        else if (appState->cli.GetInfo().format == "hsv")
        {
            auto hsv = RGBToHSL(color.r, color.g, color.b);

            std::string colorString = std::format("hsl({}, {}, {})",
                                                  std::get<0>(hsv),
                                                  std::get<1>(hsv) * 100.0,
                                                  std::get<2>(hsv) * 100.0);

            return Output(appState, colorString);
        }
        else if (appState->cli.GetInfo().format == "all")
        {
            auto hsl = RGBToHSL(color.r, color.g, color.b);
            auto hsv = RGBToHSV(color.r, color.g, color.b);

            std::string colorString = std::format(R"(#{0:06X}
#{0:06x}
    rgb({1}, {2}, {3})
    hsl({4:.1f}, {5:.1f}, {6:.1f})
    hsv({7:.1f}, {8:.1f}, {9:.1f}))",
                                      pixel,
                                      color.r, color.g, color.b,
                                      std::get<0>(hsl), std::get<1>(hsl) * 100, std::get<2>(hsl) * 100,
                                      std::get<0>(hsv), std::get<1>(hsv) * 100, std::get<2>(hsv) * 100);

            return Output(appState, colorString);
        }

        return Utils::Localize("Color/failed_to_retrieve");
    }
}
