#ifndef SCP_SCREENSHOT
#define SCP_SCREENSHOT

#include "SDL3/SDL_render.h"

#include <cstddef>
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

                std::size_t size;
            };

            static std::unique_ptr<Screenshot> CreateInstance();

            virtual void Take() = 0;

            virtual SDL_Texture *CreateTexture(SDL_Renderer *renderer) = 0;

            void ConvertPixelFormat();

            int GetWidth() { return this->_Info.width; }
            int GetHeight() { return this->_Info.height; }
            int GetPitch() { return this->_Info.pitch; }
            int GetBitsPerPixel() { return this->_Info.bitsPerPixel; }

            unsigned long GetRedMask() { return this->_Info.redMask; }
            unsigned long GetGreenMask() { return this->_Info.greenMask; }
            unsigned long GetBlueMask() { return this->_Info.blueMask; }

            uint8_t *GetPixels() { return this->_Info.pixels; }

            std::size_t GetSize() { return this->_Info.size; }

            void SetWidth(int width) { this->_Info.width = width; }
            void SetHeight(int height) { this->_Info.height = height; }
            void SetPitch(int pitch) { this->_Info.pitch = pitch; }
            void SetBitsPerPixel(int bitsPerPixel) { this->_Info.bitsPerPixel = bitsPerPixel; }
            void SetRedMask(unsigned long redMask) { this->_Info.redMask = redMask; }
            void SetGreenMask(unsigned long greenMask) { this->_Info.greenMask = greenMask; }
            void SetBlueMask(unsigned long blueMask) { this->_Info.blueMask = blueMask; }
            void SetPixels(uint8_t *pixels) { this->_Info.pixels = pixels; }
            void SetSize(std::size_t size) { this->_Info.size = size; }
        protected:
            void _Destroy();

            Info _Info;
    };
} // namespace scp

#endif // SCP_SCREENSHOT
