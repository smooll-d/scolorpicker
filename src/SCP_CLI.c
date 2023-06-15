/*
 * MIT License
 *
 * Copyright (c) 2023 Jakub Skowron
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "SCP/SCP_CLI.h"
#include "CMake/CMake_Config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CLI Global Variables
bool outputToTerminal = false;

char scpversion[] = "";

char *format = "hex";

// CLI Global Functions
void
SCP_CLI_Help()
{
    printf("Usage: scolorpicker [-h, -v, -t, -c, -f (hex, rgb, etc.)]\n\n");
    printf("Legend:\n");
    printf("    []                        Optional\n");
    printf("    ()                        Required\n\n");
    printf("Available options:\n");
    printf("    -h, --help                Displays this message and exits\n");
    printf("    -v, --version             Displays version of scolorpicker and exits\n");
    printf("    -t, --output-to-terminal  Outputs color code to terminal instead of clipboard\n");
    printf("    -c, --output-to-clipboard Outputs color code to clipboard instead of terminal  [DEFAULT]\n");
    printf("    -f, --format              Color code format\n\n");
    printf("Available color code formats:\n");
    printf("    hex                       Hexadecimal (e.g. #FFFFFF)                           [DEFAULT]\n");
    printf("    rgb                       Red, Green, Blue (e.g. rgb(255, 255, 255))\n");

    exit(0);
}

void
SCP_CLI_Version()
{
    sprintf(scpversion, "v%s.%s.%s", scpvmajor, scpvminor, scpvpatch);

    printf("scolorpicker %s - Jakub Skowron (@reallySmooll) <jakubskowron676@gmail.com>\n", scpversion);

    exit(0);
}

void
SCP_CLI_HandleArguments(const char *option, const char *value)
{
    if (strcmp(option, "--help") == 0 || strcmp(option, "-h") == 0)
    { SCP_CLI_Help(); }
    else if (strcmp(option, "--version") == 0 || strcmp(option, "-v") == 0)
    { SCP_CLI_Version(); }
    else if (strcmp(option, "--output-to-terminal") == 0 || strcmp(option, "-t") == 0)
    { outputToTerminal = true; }
    else if (strcmp(option, "--output-to-clipboard") == 0 || strcmp(option, "-c") == 0)
    { outputToTerminal = false; }
    else if (strcmp(option, "--format") == 0 || strcmp(option, "-f") == 0)
    {
        if (value == NULL)
        {
            fprintf(stderr, "Missing value for option: %s\n", option);
            exit(1);
        }
        else if (strcmp(value, "hex") == 0)
        { format = "hex"; }
        else if (strcmp(value, "rgb") == 0)
        { format = "rgb"; }
        else
        {
            fprintf(stderr, "Invalid value for option %s: %s\n", option, value);
            exit(1);
        }
    }
    else
    {
        fprintf(stderr, "Unknown option: %s\n", option);
        exit(1);
    }
}
