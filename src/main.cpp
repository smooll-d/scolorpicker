#include "CLI/SCP_CLI.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "Utils/SCP_Utils.hpp"
#include "version.hpp"
#include <X11/X.h>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <strings.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <X11/Xlib.h>

#include <format>
#include <cstdlib>
#include <iostream>

struct SCPImage
{
    int width;
    int height;
    std::uint8_t* pixels;
    int pitch;
};

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_Texture* texture = nullptr;

static SCPImage image {};

static unsigned CountTrailingZeros(int number)
{
    /* This is the function that will extract our color channels.
     *
     * First it checks if the number is 0, if it is, don't do anything, no need to shift.
     * Then, while the least significant bit of the number is 0, increment the bits by 1 and shift the number by one bit to the right.
     * We do this until the least significant bit is not 0, which means our color bits begin.
     * We can than return the bits we need to shift each color channel by.
    */

    unsigned bits = 0;

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

static std::uint8_t* CopyAndConvertImage(XImage* image)
{
    /* This is the function that will convert our image from one format to another.
     *
     * First we figure out how many bytes are in a pixel, we need this for memory allocation and offsets.
     * Next, we allocate enough memory for our pixel buffer, which is width of the image
     * multiplied by height of the image multiplied by the amount of bytes in each pixel.
     * Then we create a nested for loop to get the x and y position of the pixel in our image.
     * We get our pixel.
     * We isolate the color of the pixel.
     * Finally we place the isolated pixel color in the correct position in our buffer.
     */

    int bytesPerPixel = image->bits_per_pixel / 8;

    std::uint8_t* newPixels = new std::uint8_t[image->width * image->height * bytesPerPixel];

    for (int x = 0; x < image->width; x++)
    {
        for (int y = 0; y < image->height; y++)
        {
            // NOTE: For some reason XGetPixel() doesn't exist, so instead of using that, we'll just manually expand the macro
            unsigned long pixel = image->f.get_pixel(image, x, y);

            unsigned char blue = (pixel & image->blue_mask) >> CountTrailingZeros(image->blue_mask);
            unsigned char green = (pixel & image->green_mask) >> CountTrailingZeros(image->green_mask);
            unsigned char red = (pixel & image->red_mask) >> CountTrailingZeros(image->red_mask);

            newPixels[(x + image->width * y) * bytesPerPixel] = red;
            newPixels[(x + image->width * y) * bytesPerPixel + 1] = green;
            newPixels[(x + image->width * y) * bytesPerPixel + 2] = blue;
            newPixels[(x + image->width * y) * bytesPerPixel + 3] = 255;
        }
    }

    return newPixels;
}

static void TakeScreenshot()
{
    // Establish connection to X server
    Display* display = XOpenDisplay(nullptr);
    if (!display)
    {
        std::cerr << "Failed to open X display!\n";

        std::exit(1);
    }

    // Get the root window of display
    Window root = DefaultRootWindow(display);
    if (!root)
    {
        std::cerr << "Failed to get root window!\n";

        std::exit(1);
    }

    // Get root window's attributes (needed for width and height of display)
    XWindowAttributes rootAttributes {};
    if (!XGetWindowAttributes(display, root, &rootAttributes))
    {
        std::cerr << "Failed to get root window attributes!\n";

        std::exit(1);
    }

    // Take a screenshot
    XImage* screenshot = XGetImage(display, root, 0, 0, rootAttributes.width, rootAttributes.height, AllPlanes, ZPixmap);
    if (!screenshot)
    {
        std::cerr << "Failed to take screenshot!\n";

        std::exit(1);
    }

    // Copy pixels from our screenshot into a buffer we control and convert them into one format
    image.width = screenshot->width;
    image.height = screenshot->height;
    image.pixels = CopyAndConvertImage(screenshot);
    image.pitch = screenshot->bytes_per_line;

    // Destroy screenshot as it is not needed anymore once we transfer it to an SDL_image
    // NOTE: For some reason XDestroyImage() doesn't exist, so instead of using that, we'll just manually expand the macro
    screenshot->f.destroy_image(screenshot);

    // Close connection to X server
    XCloseDisplay(display);
}

static SDL_Texture* CreateTextureFromScreenshot()
{
    // Create surface from screenshot data
    SDL_Surface* surface = SDL_CreateSurfaceFrom(image.width, image.height, SDL_PIXELFORMAT_RGBA32, image.pixels, image.pitch);
    if (!surface)
    {
        SDL_Log("Failed to create surface from screenshot: %s", SDL_GetError());

        std::exit(1);
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        SDL_Log("Failed to create texture from screenshot: %s", SDL_GetError());

        SDL_Quit();
    }

    // Destroy surface as it is not needed anymore
    SDL_DestroySurface(surface);

    return texture;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SCP::CLI cli(argc, argv);

    TakeScreenshot();

    SDL_SetAppMetadata("scolorpicker", std::format("{}.{}.{}", SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH).c_str(), "dev.smooll.scolorpicker");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("scolorpicker", 1280, 720, SDL_WINDOW_FULLSCREEN, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    texture = CreateTextureFromScreenshot();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.key)
            {
                case SDLK_ESCAPE:
                    return SDL_APP_SUCCESS;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);

    SDL_RenderTexture(renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // Destroy texture
    SDL_DestroyTexture(texture);
}
