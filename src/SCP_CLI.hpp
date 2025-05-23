#ifndef SCP_CLI
#define SCP_CLI

#include <string>
#include <vector>
#include <iterator>

namespace SCP
{
    class CLI
    {
        public:
            struct Info
            {
                std::string format = "hex";
                std::string output = "terminal";
            };

            CLI();

            void Initialize(int argc, char* argv[]);
            void HandleParameters(std::string parameter);
            void ShowHelp();
            void ShowVersion();
        private:
            bool _FindArgument(std::string option, std::string argument);

            int _ParameterCount;

            std::vector<std::string> _Parameters;

            std::vector<std::string>::iterator _ParameterIterator;
            std::iterator_traits<decltype(_ParameterIterator)>::difference_type _ParameterDistance;

            Info _Info;
    };
} // namespace SCP

#endif // SCP_CLI
