#include "SCP/SCP_Clipboard.h"

#include <stdio.h>

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
