#include "CLI.hpp"
#include "Utils/Utils.hpp"
#include "config.hpp"

#include <algorithm>
#include <cstdlib>
#include <format>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <locale>
#include <string>

namespace scp
{
    CLI::Info CLI::_Info {};

    std::string CLI::cwd = "";

    CLI::CLI(int argc, char *argv[])
     : _ParameterCount{argc},
       _Parameters{argv, argv + argc},
       _ParameterIterator{},
       _ParameterDistance{0}
    {
        if (std::filesystem::exists(SCP_DATA_DIRECTORY_DEV))
            this->cwd = SCP_DATA_DIRECTORY_DEV;
        else
            this->cwd = SCP_DATA_DIRECTORY_REL;

        _SetLanguage();

        for (const auto &parameter : this->_Parameters)
        {
            if (parameter.starts_with("--") || parameter.starts_with("-"))
                this->_HandleParameters(parameter);
        }
    }

    void CLI::_HandleParameters(std::string parameter)
    {
        if (parameter == "--language" || parameter == "-l")
        {
            if (this->_FindArgument(parameter, "en"))
                this->_Info.language = "en";
            else if (this->_FindArgument(parameter, "pl"))
                this->_Info.language = "pl";
            else
            {
                std::string UnknownLanguageError = Utils::Localize("CLI/unknown_language");

                std::cout << Utils::ReplacePlaceholder(UnknownLanguageError,
                                                       this->_Parameters.at(this->_ParameterDistance + 1));
            }
        }
        else if (parameter == "--help" || parameter == "-h")
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
                std::cout << Utils::Localize("CLI/unknown_format");

            return;
        }
        else if (parameter == "--output" || parameter == "-o")
        {
            if (this->_FindArgument(parameter, "terminal"))
                this->_Info.output = "terminal";
            else if (this->_FindArgument(parameter, "clipboard"))
                this->_Info.output = "clipboard";
            else
                std::cout << Utils::Localize("CLI/unknown_output");

            return;
        }
        else
        {
            std::string unknownParameter = Utils::Localize("CLI/unknown_parameter");

            std::cerr << Utils::ReplacePlaceholder(unknownParameter, parameter);

            std::exit(1);
        }
    }

    void CLI::_ShowHelp()
    {
        std::cout << Utils::Localize("CLI/help");
    }

    void CLI::_ShowVersion()
    {
        std::cout << std::format("scolorpicker v{}.{}.{}",
                                 SCP_VERSION_MAJOR, SCP_VERSION_MINOR, SCP_VERSION_PATCH)
                  << '\n';
    }

    void CLI::_SetLanguage()
    {
        if (std::locale("").name() == "pl_PL" || std::locale("").name() == "pl_PL.UTF-8")
            this->_Info.language = "pl";
        else
            this->_Info.language = "en";
    }

    bool CLI::_FindArgument(std::string option, std::string argument)
    {
        this->_ParameterIterator = std::find(this->_Parameters.begin(), this->_Parameters.end(), option);
        this->_ParameterDistance = std::distance(this->_Parameters.begin(), this->_ParameterIterator);

        return this->_Parameters.at(this->_ParameterDistance + 1) == argument;
    }
} // namespace scp