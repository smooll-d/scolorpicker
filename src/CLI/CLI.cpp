#include "CLI.hpp"
#include "Utils/Utils.hpp"
#include "version.hpp"

#include <algorithm>
#include <cstdlib>
#include <format>
#include <iostream>
#include <iterator>

namespace scp
{
    CLI::CLI(int argc, char *argv[])
     : _ParameterCount{argc},
       _Parameters{argv, argv + argc},
       _ParameterIterator{},
       _ParameterDistance{0}
    {
        for (const auto &parameter : _Parameters)
        {
            if (parameter.starts_with("--") || parameter.starts_with("-"))
            { _HandleParameters(parameter); }
        }
    }

    void CLI::_HandleParameters(std::string parameter)
    {
        if (parameter == "--help" || parameter == "-h")
        {
            _ShowHelp();

            std::exit(0);
        }
        else if (parameter == "--version" || parameter == "-v")
        {
            _ShowVersion();

            std::exit(0);
        }
        else if (parameter == "--format" || parameter == "-f")
        {
            if (_FindArgument(parameter, "hex"))
            { _Info.format = "hex"; }
            else if (_FindArgument(parameter, "lhex"))
            { _Info.format = "lhex"; }
            else if (_FindArgument(parameter, "rgb"))
            { _Info.format = "rgb"; }
            else
            {
                std::cout << "Unknown format!\n";
                std::cout << "Color will be formatted in \"hex\"\n";
            }

            return;
        }
        else if (parameter == "--output" || parameter == "-o")
        {
            if (_FindArgument(parameter, "terminal"))
            { _Info.output = "terminal"; }
            else if (_FindArgument(parameter, "clipboard"))
            { _Info.output = "clipboard"; }
            else
            {
                std::cout << "Unknown output!\n";
                std::cout << "Color will be output to \"terminal\"\n";
            }

            return;
        }
        else
        {
            std::cerr << "Unknown parameter: \"" << parameter << "\"!\n";
            std::cerr << "See \"scolorpicker --help\" for usage.\n";

            std::exit(1);
        }
    }

    void CLI::_ShowHelp()
    {
        std::string uppercase = Utils::TRed("#FF0000");
        std::string lowercase = Utils::TRed("#ff0000");
        std::string rgb = Utils::TRed("rgb(255, 0, 0)");

        std::cout << std::format(R"(Usage: scolorpicker [Options]

Options:
    --help,    -h          Show this message and exit
    --version, -v          Show version + other info and exit
    --format,  -f <Format> Choose how selected color is formatted
    --output,  -o <Output> Choose where color is sent to

Format:
    hex                    Hexadecimal (e.g. {})                  [DEFAULT]
    lhex                   Hexadecimal in lowercase (e.g. {})
    rgb                    RGB (e.g. {})

Output:
    terminal               Print color in specified format to terminal  [DEFAULT]
    clipboard              Send color in specified format to clipboard))", uppercase, lowercase, rgb)
                  << '\n';
    }

    void CLI::_ShowVersion()
    {
        std::string heart = Utils::TRed("<3", "foreground");

        std::cout << std::format(R"(scolorpicker v{}.{}.{}
Made with {} by Jakub Skowron (@smooll-d))", SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH, heart)
                  << '\n';
    }

    bool CLI::_FindArgument(std::string option, std::string argument)
    {
        _ParameterIterator = std::find(_Parameters.begin(), _Parameters.end(), option);
        _ParameterDistance = std::distance(_Parameters.begin(), _ParameterIterator);

        return _Parameters.at(_ParameterDistance + 1) == argument;
    }
} // namespace scp
