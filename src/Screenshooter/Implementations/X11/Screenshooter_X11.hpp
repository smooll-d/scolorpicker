#ifndef SCP_SCREENSHOOTER_X11
#define SCP_SCREENSHOOTER_X11

#include "Screenshooter/Screenshooter.hpp"
#include "config.hpp"

#include <SDL3/SDL_render.h>

#ifdef SCP_ENABLE_XCB
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif // SCP_ENABLE_XCB

namespace scp
{
    class Screenshooter_X11 : public Screenshooter
    {
        public:
            void Take() override;

            SDL_Texture *CreateTexture(SDL_Renderer *renderer) override;
#ifdef SCP_ENABLE_XCB
        private:
            xcb_screen_t *GetScreen(xcb_connection_t *connection, int screen);

            xcb_visualtype_t *GetVisualType(xcb_connection_t *connection, xcb_screen_t *screen);
#endif // SCP_ENABLE_XCB
    };
} // namespace scp

#endif // SCP_SCREENSHOOTER_X11
