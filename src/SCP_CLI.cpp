#include <SCP/SCP_CLI.hpp>
#include <SCP/metadata.hpp>
#include <SCP/SCP_Utils.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <format>

namespace SCP
{
    void CLI::Create(int argc, char* argv[])
    {
        mParameterCount = argc;
        mParameters = { argv, argv + argc };

        for (const auto& parameter : mParameters)
        {
            if (parameter.starts_with("--") || parameter.starts_with("-"))
            { HandleParameters(parameter); }
        }
    }

    void CLI::HandleParameters(std::string_view parameter)
    {
        if (parameter == "--help" || parameter == "-h")
        {
            ShowHelp();

            std::exit(0);
        }
        else if (parameter == "--version" || parameter == "-v")
        {
            ShowVersion();

            std::exit(0);
        }
        else if (parameter == "--format" || parameter == "-f")
        {
            if (mFindArgument("hex"))
            { mInfo.format = "hex"; }
            else if (mFindArgument("lhex"))
            { mInfo.format = "lhex"; }
            else if (mFindArgument("rgb"))
            { mInfo.format = "rgb"; }
            else
            {
                std::cout << "Unknown format!\n";
                std::cout << "Color will be formatted in \"hex\"\n";
            }

            return;
        }
        else if (parameter == "--output" || parameter == "-o")
        {
            if (mFindArgument("terminal"))
            { mInfo.output = "terminal"; }
            else if (mFindArgument("clipboard"))
            { mInfo.output = "clipboard"; }
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

    void CLI::ShowHelp()
    {
        std::string whiteUppercase = tredbg("#FF0000");
        std::string whiteLowercase = tredbg("#ff0000");
        std::string whiteRGB = tredbg("rgb(255, 0, 0)");

        std::cout << std::format(R"(Usage: scolorpicker [options]

Options:
    --help,    -h          Show this message and exit
    --version, -v          Show version + other info and exit
    --format,  -f [format] Choose how selected color is formatted
    --output,  -o [output] Choose where color is sent to

Format:
    hex                    Hexadecimal (e.g. {})                  [DEFAULT]
    lhex                   Hexadecimal in lowercase (e.g. {})
    rgb                    RGB (e.g. {})

Output:
    terminal               Print color in specified format to terminal [DEFAULT]
    clipboard              Send color in specified format to clipboard))", whiteUppercase, whiteLowercase, whiteRGB)
                  << '\n';
    }

    void CLI::ShowVersion()
    {
        std::string heart = tredfg("\u2665");

        std::cout << std::format(R"(scolorpicker v{}.{}.{}
Made with {} by Jakub Skowron (@smooll-d))", SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH, heart)
                  << '\n';
    }

    bool CLI::mFindArgument(std::string_view argument)
    { return std::find(mParameters.begin(), mParameters.end(), argument) != mParameters.end(); }
}
