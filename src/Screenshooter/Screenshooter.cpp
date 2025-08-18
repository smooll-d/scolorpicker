#include "Screenshooter.hpp"
#include "config.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sys/types.h>

#ifdef SCP_ENABLE_X11
#include "Implementations/X11/Screenshooter_X11.hpp"
#elif defined SCP_ENABLE_WAYLAND
#include "Implementations/Wayland/Screenshooter_Wayland.hpp"
#elif defined SCP_ENABLE_LINUX
#include "Implementations/X11/Screenshooter_X11.hpp"
#include "Implementations/Wayland/Screenshooter_Wayland.hpp"
#endif // SCP_ENABLE_X11

namespace scp
{
    std::unique_ptr<Screenshooter> Screenshooter::CreateInstance()
    {
#ifdef SCP_ENABLE_X11
        return std::make_unique<Screenshooter_X11>();
#elif defined SCP_ENABLE_WAYLAND
        return std::make_unique<Screenshooter_Wayland>();
#elif defined SCP_ENABLE_LINUX
        std::string sessionType = std::getenv("XDG_SESSION_TYPE");

        if (std::getenv("DISPLAY") && sessionType.compare("x11") == 0)
        {
            return std::make_unique<Screenshooter_X11>();
        }
        else if (std::getenv("WAYLAND_DISPLAY") && sessionType.compare("wayland") == 0)
        {
            return std::make_unique<Screenshooter_Wayland>();
        }
#endif

        std::cerr << "Failed to determine platform!\n";

        return nullptr;
    }
} // namespace scp
