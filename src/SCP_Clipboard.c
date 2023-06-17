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
