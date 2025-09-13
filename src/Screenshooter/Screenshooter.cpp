#include "Screenshooter.hpp"
#include "Utils/Utils.hpp"
#include "config.hpp"

#ifdef SCP_ENABLE_X11
#include "Implementations/X11/Screenshooter_X11.hpp"
#elif defined SCP_ENABLE_WAYLAND
#include "Implementations/Wayland/Screenshooter_Wayland.hpp"
#elif defined SCP_ENABLE_LINUX
#include "Implementations/X11/Screenshooter_X11.hpp"
#include "Implementations/Wayland/Screenshooter_Wayland.hpp"
#endif // SCP_ENABLE_X11

#include <iostream>
#include <memory>
#include <sys/types.h>

namespace scp
{
    std::unique_ptr<Screenshooter> Screenshooter::CreateInstance()
    {
#ifdef SCP_ENABLE_X11
        return std::make_unique<Screenshooter_X11>();
#elif defined SCP_ENABLE_WAYLAND
        return std::make_unique<Screenshooter_Wayland>();
#elif defined SCP_ENABLE_LINUX
        if (Utils::CheckSession() == 0)
            return std::make_unique<Screenshooter_X11>();
        else if (Utils::CheckSession() == 1)
            return std::make_unique<Screenshooter_Wayland>();
#endif

        std::cerr << Utils::Localize("Screenshooter/failed_to_determine_platform");

        return nullptr;
    }
} // namespace scp