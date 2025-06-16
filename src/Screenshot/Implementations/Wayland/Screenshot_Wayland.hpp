#ifndef SCP_SCREENSHOT_WAYLAND
#define SCP_SCREENSHOT_WAYLAND

#include "Screenshot/Screenshot.hpp"

namespace scp
{
    class Screenshot_Wayland : public Screenshot
    {
        public:
            void Take() override;
    };
} // namespace scp

#endif // SCP_SCREENSHOT_WAYLAND
