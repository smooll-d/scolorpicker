#ifndef SCP_UI
#define SCP_UI

#include <SDL3/SDL_render.h>

#include <utility>

namespace scp
{
    class UI
    {
        public:
            UI(SDL_Renderer *renderer);
            UI(const UI &other);
            UI(UI &&other);

            void Circle(std::pair<float, float> center, float radius, bool filled = false);
            void RoundedRectangle(std::pair<float, float> position,
                                  std::pair<float, float> size,
                                  float radius,
                                  bool filled = false);
        private:
            SDL_Renderer *_Renderer;
    };
}

#endif // SCP_UI