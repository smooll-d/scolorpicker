#include "UI.hpp"

#include <SDL3/SDL_render.h>
#include <vector>

namespace scp
{
    UI::UI(SDL_Renderer *renderer)
     : _Renderer{renderer}
    {}

    UI::UI(const UI &other)
     : _Renderer{other._Renderer}
    {}

    UI::UI(UI &&other)
     : _Renderer{std::move(other._Renderer)}
    {}

    void UI::Circle(std::pair<float, float> center, float radius, bool filled)
    {
        std::vector<SDL_FPoint> points;

        float d = 1.0f - radius; // decision variable

        float x = 0.0f;
        float y = radius;
        float cx = center.first;
        float cy = center.second;

        while (x <= y)
        {
            points.push_back({ cx + x, cy - y });
            points.push_back({ cx + x, cy + y });
            points.push_back({ cx - x, cy - y });
            points.push_back({ cx - x, cy + y });
            points.push_back({ cx + y, cy - x });
            points.push_back({ cx + y, cy + x });
            points.push_back({ cx - y, cy - x });
            points.push_back({ cx - y, cy + x });

            x++;
            if (d < 0.0f)
                d += 2.0f * x + 1.0f;
            else
            {
                y--;
                d += 2.0f * (x - y) + 1.0f;
            }
        }

        points.shrink_to_fit();

        if (filled)
            SDL_RenderLines(_Renderer, points.data(), points.size());
        else
            SDL_RenderPoints(_Renderer, points.data(), points.size());
    }

    void UI::RoundedRectangle(std::pair<float, float> position,
                              std::pair<float, float> size,
                              float radius,
                              bool filled)
    {
        float x = position.first;
        float y = position.second;
        float width = size.first;
        float height = size.second;

        SDL_FRect vertical
        {
            x, y + radius,
            width + 1.5f, height - (radius * 2.0f)
        };
        SDL_FRect horizontal
        {
            x + radius, y,
            width - (radius * 2.0f), height + 1.5f
        };

        if (filled)
        {
            Circle({ x + radius,         y + radius          }, radius, true); // Top-left
            Circle({ x + width - radius, y + radius          }, radius, true); // Bottom-left
            Circle({ x + radius,         y + height - radius }, radius, true); // Top-right
            Circle({ x + width - radius, y + height - radius }, radius, true); // Bottom-right

            SDL_RenderFillRect(_Renderer, &vertical);
            SDL_RenderFillRect(_Renderer, &horizontal);
        }
        else
        {
            Circle({ x + radius,         y + radius          }, radius); // Top-left
            Circle({ x + width - radius, y + radius          }, radius); // Bottom-left
            Circle({ x + radius,         y + height - radius }, radius); // Top-right
            Circle({ x + width - radius, y + height - radius }, radius); // Bottom-right

            SDL_RenderRect(_Renderer, &vertical);
            SDL_RenderRect(_Renderer, &horizontal);
        }
    }
}