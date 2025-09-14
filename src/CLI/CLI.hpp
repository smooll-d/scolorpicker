#ifndef SCP_CLI
#define SCP_CLI

#include <string>
#include <vector>
#include <iterator>

namespace scp
{
    class CLI
    {
        public:
            struct Info
            {
                std::string format = "hex";
                std::string output = "terminal";
                std::string language = "en";
            };

            CLI() = default;
            CLI(int argc, char *argv[]);

            static const Info &GetInfo() { return _Info; }

            static std::string cwd;
        private:
            void _ShowHelp();
            void _ShowVersion();
            void _HandleParameters(std::string parameter);
            void _SetLanguage();

            bool _FindArgument(std::string option, std::string argument);

            static Info _Info;

            int _ParameterCount;

            std::vector<std::string> _Parameters;

            std::vector<std::string>::iterator _ParameterIterator;
            std::iterator_traits<decltype(_ParameterIterator)>::difference_type _ParameterDistance;
    };
} // namespace scp

#endif // SCP_CLI