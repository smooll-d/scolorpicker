#ifndef SCP_SCREENSHOOTER
#define SCP_SCREENSHOOTER

#include <SDL3/SDL_render.h>

#include <memory>

namespace scp
{
    class Screenshooter
    {
        public:
            static std::unique_ptr<Screenshooter> CreateInstance();

            virtual void Take() = 0;

            virtual SDL_Texture *CreateTexture(SDL_Renderer *renderer) = 0;
        protected:
    };
} // namespace scp

#endif // SCP_SCREENSHOOTER
