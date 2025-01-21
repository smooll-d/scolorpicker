#ifndef SCP_UTILS
#define SCP_UTILS

#include <string>
#include <vector>

namespace SCP
{
    namespace Utils
    {
        std::string tred(std::string_view str, std::string_view ground = "background");

        std::vector<int> RGB(std::vector<unsigned short> colors);
    }
}

#endif // SCP_UTILS
