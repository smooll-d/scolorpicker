#ifndef SCP_CLIPBOARD_H
#define SCP_CLIPBOARD_H

#include <X11/Xlib.h>

void SCP_Clipboard_GetOwners(Display *display);
void SCP_Clipboard_CopyPixelColor(Display *display, int x, int y, XColor *color);

#endif // SCP_CLIPBOARD_H
