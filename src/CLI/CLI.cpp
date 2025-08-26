#include "CLI.hpp"
#include "Utils/Utils.hpp"
#include "config.hpp"

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
        for (const auto &parameter : this->_Parameters)
        {
            if (parameter.starts_with("--") || parameter.starts_with("-"))
                this->_HandleParameters(parameter);
        }
    }

    void CLI::_HandleParameters(std::string parameter)
    {
        if (parameter == "--help" || parameter == "-h")
        {
            this->_ShowHelp();

            std::exit(0);
        }
        else if (parameter == "--version" || parameter == "-v")
        {
            this->_ShowVersion();

            std::exit(0);
        }
        else if (parameter == "--format" || parameter == "-f")
        {
            if (this->_FindArgument(parameter, "hex"))
                this->_Info.format = "hex";
            else if (this->_FindArgument(parameter, "lhex"))
                this->_Info.format = "lhex";
            else if (this->_FindArgument(parameter, "rgb"))
                this->_Info.format = "rgb";
            else if (this->_FindArgument(parameter, "hsl"))
                this->_Info.format = "hsl";
            else if (this->_FindArgument(parameter, "hsv"))
                this->_Info.format = "hsv";
            else if (this->_FindArgument(parameter, "all"))
                this->_Info.format = "all";
            else
            {
                std::cout << "Unknown format!\n";
                std::cout << "Color will be formatted in \"hex\"\n";
            }

            return;
        }
        else if (parameter == "--output" || parameter == "-o")
        {
            if (this->_FindArgument(parameter, "terminal"))
                this->_Info.output = "terminal";
            else if (this->_FindArgument(parameter, "clipboard"))
                this->_Info.output = "clipboard";
            else
            {
                std::cout << "Unknown output type!\n";
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
        std::cout << R"(scolorpicker [Options]

Options:
    --help,    -h          Show this message and exit
    --version, -v          Show version + other info and exit
    --format,  -f <Format> How color value is formatted
    --output,  -o <Output> Where formatted color value is sent to

Format:
    hex                    Hexadecimal (e.g. #FF0000)                   [DEFAULT]
    lhex                   Hexadecimal in lowercase (e.g. #ff0000)
    rgb                    Red Green Blue (e.g. rgb(255, 0, 0))
    hsl                    Hue Saturation Lightness (e.g. hsl(0, 100, 50))
    hsv                    Hue Saturation Value (e.g. hsv(0, 100, 100))
    all                    All formats separated by newlines

Output:
    terminal               Print color in specified format to terminal  [DEFAULT]
    clipboard              Send color in specified format to clipboard)" << '\n';
    }

    void CLI::_ShowVersion()
    {
        std::string heart = Utils::TRed("<3", "foreground");

        std::cout << std::format("scolorpicker v{}.{}.{}",
                                 SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH)
                  << "\n\n";
        std::cout << std::format("Made with {} by Jakub Skowron (@smooll-d)", heart) << '\n';
    }

    bool CLI::_FindArgument(std::string option, std::string argument)
    {
        this->_ParameterIterator = std::find(this->_Parameters.begin(), this->_Parameters.end(), option);
        this->_ParameterDistance = std::distance(this->_Parameters.begin(), this->_ParameterIterator);

        return this->_Parameters.at(this->_ParameterDistance + 1) == argument;
    }
} // namespace scp