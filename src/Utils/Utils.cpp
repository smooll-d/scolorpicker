#include "Utils.hpp"
#include "CLI/CLI.hpp"

#include <SDL3/SDL.h>

#include <stb_image/stb_image.h>

#include <format>
#include <fstream>
#include <locale>
#include <sstream>
#include <string>
#include <filesystem>

namespace scp
{
    std::string Utils::TRed(std::string_view str, std::string_view ground)
    {
        std::stringstream ss;

        if (ground == "background")
        ss << "\e[41m" << str << "\e[0m";
        else if (ground == "foreground")
        ss << "\e[1;31m" << str << "\e[0m";

        return ss.str();
    }

    void Utils::TestSDL(SDL_Renderer *renderer)
    {
        const double now = ((double)SDL_GetTicks()) / 1000.0;

        const float red = (float)(0.5 + 0.5 * SDL_sin(now));
        const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
        const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

        SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);
    }

    // Returns index of the first bit different than 0 in a byte sequence, counting from the least significant bit.
    unsigned int Utils::CountTrailingZeroes(int number)
    {
        unsigned int bits = 0;

        if (number)
        {
            while ((number & 1) == 0)
            {
                bits++;
                number >>= 1;
            }
        }

        return bits;
    }

    int Utils::CheckSession()
    {
        std::string session = std::getenv("XDG_SESSION_TYPE");

        if (std::getenv("DISPLAY") && session == "x11")
            return 0;
        else if (std::getenv("WAYLAND_DISPLAY") && session == "wayland")
            return 1;

        return -1;
    }

    std::string Utils::Localize(std::string_view messageName)
    {
        std::fstream messageFile;

        std::string line = "";
        std::string message = "";

        if (CLI::GetInfo().language == "en")
            messageFile.open(std::format("{}/messages/en/{}.txt", CLI::cwd, messageName),
                             messageFile.binary | messageFile.in);
        else if (CLI::GetInfo().language == "pl")
            messageFile.open(std::format("{}/messages/pl/{}.txt", CLI::cwd, messageName),
                             messageFile.binary | messageFile.in);

        if (!messageFile.is_open())
        {
            std::string failedMessage = Localize("Utils/message_not_loaded");

            return ReplacePlaceholder(failedMessage, messageName);
        }

        while (std::getline(messageFile, line))
            message += line + '\n';

        messageFile.close();

        return message;
    }

    std::string Utils::ReplacePlaceholder(std::string placeholder, std::string_view actual)
    {
        auto placeholderBegin = std::find(placeholder.begin(), placeholder.end(), '<');
        auto placeholderEnd = std::find(placeholder.begin(), placeholder.end(), '>');

        return placeholder.replace(placeholderBegin, std::next(placeholderEnd), actual);
    }

    SDL_Surface *Utils::CreateSurfaceFromSTBI(std::filesystem::path filepath)
    {
        int width;
        int height;
        int bytesPerPixel;
        int pitch;
        int redMask;
        int greenMask;
        int blueMask;
        int alphaMask;

        unsigned char *pixels = stbi_load(filepath.c_str(), &width, &height, &bytesPerPixel, 0);

        pitch = width * bytesPerPixel;
        pitch = (pitch + 3) & ~3;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        redMask = 0x000000FF;
        greenMask = 0x0000FF00;
        blueMask = 0x00FF0000;
        alphaMask = (bytesPerPixel == 4) ? 0xFF000000 : 0;
#else
        int s = (bytesPerPixel == 4) ? 0 : 8;
        redMask = 0xFF000000 >> s;
        greenMask = 0x00FF0000 >> s;
        blueMask = 0x0000FF00 >> s;
        alphaMask = 0x000000FF >> s;
#endif

        SDL_PixelFormat pixelFormat = SDL_GetPixelFormatForMasks(bytesPerPixel * 8,
                                                                 redMask,
                                                                 greenMask,
                                                                 blueMask,
                                                                 alphaMask);

        SDL_Surface *surface = SDL_CreateSurfaceFrom(width, height, pixelFormat, pixels, pitch);
        if (!surface)
        {
            std::string surfaceSTBIError = Utils::Localize("Utils/surface_stbi_creation");

            SDL_Log("%s", Utils::ReplacePlaceholder(surfaceSTBIError, SDL_GetError()).c_str());

            stbi_image_free(pixels);

            SDL_Quit();
        }

        return surface;
    }
} // namespace scp