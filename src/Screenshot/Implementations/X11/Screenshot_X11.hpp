#ifndef SCP_SCREENSHOT_X11
#define SCP_SCREENSHOT_X11

#include "Screenshot/Screenshot.hpp"

namespace scp
{
    class Screenshot_X11 : public Screenshot
    {
        public:
            void Take() override;
    };
} // namespace scp

#endif
