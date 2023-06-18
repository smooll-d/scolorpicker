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

#include "SCP/SCP.h"
#include "SCP/SCP_CLI.h"
#include "SCP/SCP_Clipboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
SCP_Clipboard_GetOwners(Display *display)
{
    Window owner;
    Atom sel;
    char *selections[] = { "PRIMARY", "SECONDARY", "CLIPBOARD", "FOOBAR" };
    size_t i;

    for (i = 0; i < sizeof(selections) / sizeof(selections[0]); i++)
    {
        sel = XInternAtom(display, selections[i], False);
        owner = XGetSelectionOwner(display, sel);
        printf("Owner of '%s': 0x%lX\n", selections[i], owner);
    }
}

void
SCP_Clipboard_CopyPixelColor(Display *display, int x, int y, XColor *color)
{
    SCP_GetPixelColor(display, x, y, color);

    SCP_ChooseFormat(format);

    FILE *pipe = popen("xsel -b", "w");
    if (pipe == NULL)
    {
        fprintf(stderr, "Failed to open pipe to xsel!\n");
        return;
    }

    fprintf(pipe, "%s", hex);
    pclose(pipe);
}
