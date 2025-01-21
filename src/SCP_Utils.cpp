#include <SCP/SCP_Utils.hpp>

#include <sstream>
#include <vector>

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

        std::vector<int> RGB(std::vector<unsigned short> colors)
        { return { colors.at(0) / 256, colors.at(1) / 256, colors.at(2) / 256 }; }
    }
}
