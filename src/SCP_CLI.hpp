#ifndef SCP_CLI
#define SCP_CLI

#include <string>
#include <vector>
#include <iterator>

namespace SCP
{
    class CLI
    {
        private:
            struct Info
            {
                std::string format = "hex";
                std::string output = "terminal";
            };
        public:
            CLI(int argc, char* argv[]);

            const Info& GetInfo() { return _Info; }
        private:
            bool _FindArgument(std::string option, std::string argument);
            void _ShowHelp();
            void _ShowVersion();
            void _HandleParameters(std::string parameter);

            int _ParameterCount;

            std::vector<std::string> _Parameters;

            std::vector<std::string>::iterator _ParameterIterator;
            std::iterator_traits<decltype(_ParameterIterator)>::difference_type _ParameterDistance;

            Info _Info;
    };
} // namespace SCP

#endif // SCP_CLI
