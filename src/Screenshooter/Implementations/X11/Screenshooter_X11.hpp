#ifndef SCP_SCREENSHOOTER_X11
#define SCP_SCREENSHOOTER_X11

#include "Screenshooter/Screenshooter.hpp"
#include "config.hpp"

#ifdef SCP_ENABLE_XCB
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif // SCP_ENABLE_XCB

namespace scp
{
    class Screenshooter_X11 : public Screenshooter
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

            void Take() override;

            SDL_Texture *CreateTexture(SDL_Renderer *renderer) override;

            void ConvertPixelFormat();
        private:
            void _Destroy();

#ifdef SCP_ENABLE_XCB
            xcb_screen_t *_GetScreen(xcb_connection_t *connection, int screen);

            xcb_visualtype_t *_GetVisualType(xcb_connection_t *connection, xcb_screen_t *screen);
#endif // SCP_ENABLE_XCB

            Info _Info;
    };
} // namespace scp

#endif // SCP_SCREENSHOOTER_X11