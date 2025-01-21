#include <SCP/SCP_Utils.hpp>

#include <sstream>

namespace SCP
{
    namespace Utils
    {
        std::string tred(std::string_view str, std::string_view ground)
        {
            std::stringstream ss;

            if (ground == "background")
            { ss << "\e[41m" << str << "\e[0m"; }
            else if (ground == "foreground")
            { ss << "\e[1;31m" << str << "\e[0m"; }

            return ss.str();
        }
    }
}
