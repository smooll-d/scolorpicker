#include <SCP/SCP_Utils.hpp>

#include <sstream>

namespace SCP
{
    std::string tredbg(std::string_view str)
    {
        std::stringstream ss;

        ss << "\e[41m" << str << "\e[0m";

        return ss.str();
    }

    std::string tredfg(std::string_view str)
    {
        std::stringstream ss;

        ss << "\e[1;31m" << str << "\e[0m";

        return ss.str();
    }
}
