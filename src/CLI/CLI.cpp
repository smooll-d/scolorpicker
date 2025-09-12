#include "CLI.hpp"
#include "Utils/Utils.hpp"
#include "config.hpp"

#include <algorithm>
#include <cstdlib>
#include <format>
#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <locale>
#include <string>

namespace scp
{
    CLI::CLI(int argc, char *argv[])
     : _Info{},
       _ParameterCount{argc},
       _Parameters{argv, argv + argc},
       _ParameterIterator{},
       _ParameterDistance{0}
    {
        if (std::filesystem::exists(SCP_DATA_DIRECTORY_DEV))
            this->cwd = SCP_DATA_DIRECTORY_DEV;
        else
            this->cwd = SCP_DATA_DIRECTORY_REL;

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
        std::fstream helpfile;

        std::string help = "";

        if (std::locale("").name() == "pl_PL" || std::locale("").name() == "pl_PL.UTF-8")
            // helpfile.open(std::format("{}/data/messages/en/help.txt", cwd), helpfile.binary | helpfile.in);
            helpfile.open("data/messages/pl/help.txt", helpfile.binary | helpfile.in);
        else
            // helpfile.open(std::format("{}/data/messages/en/help.txt", cwd), helpfile.binary | helpfile.in);
            helpfile.open("data/messages/en/help.txt", helpfile.binary | helpfile.in);

        if (!helpfile.is_open())
            std::cout << "couldn't open file!\n";

        while (std::getline(helpfile, help))
            std::cout << help << '\n';

        helpfile.close();
    }

    void CLI::_ShowVersion()
    {
        std::cout << std::format("scolorpicker v{}.{}.{}",
                                 SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH)
                  << "\n\n";
    }

    bool CLI::_FindArgument(std::string option, std::string argument)
    {
        this->_ParameterIterator = std::find(this->_Parameters.begin(), this->_Parameters.end(), option);
        this->_ParameterDistance = std::distance(this->_Parameters.begin(), this->_ParameterIterator);

        return this->_Parameters.at(this->_ParameterDistance + 1) == argument;
    }
} // namespace scp