#ifndef SCP_SCREENSHOT
#define SCP_SCREENSHOT

#include "SDL3/SDL_render.h"

#include <cstdint>
#include <memory>

namespace scp
{
    class Screenshot
    {
        public:
            struct Info
            {
                int width;
                int height;
                int pitch;
                int bitsPerPixel;

                unsigned long redMask;
                unsigned long greenMask;
                unsigned long blueMask;

                uint8_t *pixels;
            };

            static std::unique_ptr<Screenshot> CreateInstance();

            virtual void Take() = 0;

            SDL_Texture *CreateTexture(SDL_Renderer *renderer);

            void ConvertPixelFormat();

            int GetWidth() { return this->_Info.width; }
            int GetHeight() { return this->_Info.height; }
            int GetPitch() { return this->_Info.pitch; }
            int GetBitsPerPixel() { return this->_Info.bitsPerPixel; }

            unsigned long GetRedMask() { return this->_Info.redMask; }
            unsigned long GetGreenMask() { return this->_Info.greenMask; }
            unsigned long GetBlueMask() { return this->_Info.blueMask; }

            uint8_t *GetPixels() { return this->_Info.pixels; }

            void SetWidth(int width) { this->_Info.width = width; }
            void SetHeight(int height) { this->_Info.height = height; }
            void SetPitch(int pitch) { this->_Info.pitch = pitch; }
            void SetBitsPerPixel(int bitsPerPixel) { this->_Info.bitsPerPixel = bitsPerPixel; }
            void SetRedMask(unsigned long redMask) { this->_Info.redMask = redMask; }
            void SetGreenMask(unsigned long greenMask) { this->_Info.greenMask = greenMask; }
            void SetBlueMask(unsigned long blueMask) { this->_Info.blueMask = blueMask; }
            void SetPixels(uint8_t *pixels) { this->_Info.pixels = pixels; }
        private:
            void _Destroy();

            Info _Info;
    };
} // namespace scp

#endif // SCP_SCREENSHOT
