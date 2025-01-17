#ifndef SCP_CLI
#define SCP_CLI

#include <string_view>
#include <vector>

namespace SCP
{
    class CLI
    {
        public:
            struct Info
            {
                std::string_view format = "hex";
                std::string_view output = "terminal";
            };

            void Create(int argc, char* argv[]);
            void HandleParameters(std::string_view parameter);
            void ShowHelp();
            void ShowVersion();
        private:
            bool mFindArgument(std::string_view argument);

            int mParameterCount;

            std::vector<std::string_view> mParameters;

            Info mInfo;
    };
}

#endif // SCP_CLI
