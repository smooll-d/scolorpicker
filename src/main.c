#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

Display *dpy;

int screen;
int x;
int y;

Window root;
Window pixelWindow;

XEvent event;

XColor color;

Cursor cursor;

GC gc;

char *hex;

void
SCPInit()
{
    dpy = XOpenDisplay(NULL);

    if (dpy == NULL)
    {
        fprintf(stderr, "Unable to open display!\n");
        exit(1);
    }

    screen = XDefaultScreen(dpy);
    root   = XDefaultRootWindow(dpy);
    cursor = XCreateFontCursor(dpy, XC_crosshair);

    hex = malloc(8 * sizeof(char));
}

void
SCPGetPixelColor(Display *display, int x, int y, XColor *color)
{
    XImage *image;
    image = XGetImage(display, root, x, y, 1, 1, AllPlanes, ZPixmap);
    if (image == NULL)
    {
        XCloseDisplay(display);
        exit(1);
    }

    color->pixel = XGetPixel(image, 0, 0);

    XFree(image);

    XQueryColor(display, XDefaultColormap(display, screen), color);
}

void
SCPPrintPixelColor(Display *display, int x, int y, XColor *color)
{
    SCPGetPixelColor(display, x, y, color);

    printf("#%06lX\n", color->pixel);
}

void
SCPCreatePixelWindow(Display *display, XColor *color)
{
    int width = 100;
    int height = 100;

    Window rootReturn, childReturn;
    int rootXReturn, rootYReturn;
    int childXReturn, childYReturn;
    unsigned int mask;

    XQueryPointer(display, root, &rootReturn, &childReturn, &rootXReturn, &rootYReturn, &childXReturn, &childYReturn, &mask);

    SCPGetPixelColor(dpy, rootXReturn, rootYReturn, color);

    pixelWindow = XCreateSimpleWindow(display, root, rootXReturn + 10, rootYReturn + 10, width, height, 0, color->pixel, color->pixel);
    gc = XCreateGC(dpy, pixelWindow, 0, 0);

    Atom bypassCompositor = XInternAtom(display, "_NET_WM_BYPASS_COMPOSITOR", False);
    if (bypassCompositor == None)
    {
        printf("_NET_WM_BYPASS_COMPOSITOR\n");
        XCloseDisplay(display);
        exit(1);
    }

    unsigned long value = 1;
    XChangeProperty(display, pixelWindow, bypassCompositor, XA_CARDINAL, 32, PropModeReplace, (const unsigned char *)&value, 1);

    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    XChangeWindowAttributes(display, pixelWindow, CWOverrideRedirect, &attributes);

    XMapWindow(display, pixelWindow);
}

void
SCPCopyPixelColorToClipboard(Display *display, int x, int y, XColor *color)
{
    SCPGetPixelColor(display, x, y, color);

    sprintf(hex, "#%06lX", color->pixel);

    FILE *clipboard = popen("xclip -selection clipboard", "w");
    if (clipboard == NULL)
    {
        fprintf(stderr, "Failed to open clipboard!\n");
        exit(1);
    }

    size_t len = strlen(hex);
    if (fwrite(hex, sizeof(char), len, clipboard) < len)
    {
        fprintf(stderr, "Failed to write to clipboard!\n");
        exit(1);
    }

    pclose(clipboard);
}

void
SCPClose()
{
    XUngrabPointer(dpy, CurrentTime);
    XUnmapWindow(dpy, pixelWindow);
    XDestroyWindow(dpy, pixelWindow);
    XFreeCursor(dpy, cursor);

    XCloseDisplay(dpy);
    exit(0);
}

void
SCPEventLoop()
{
    while (1)
    {
        XGrabPointer(dpy, root, True, PointerMotionMask | ButtonPressMask, GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime);

        XNextEvent(dpy, &event);

        switch (event.type)
        {
            case ButtonPress:
            {
                switch (event.xbutton.button)
                {
                    case Button1:
                    {
                        SCPCopyPixelColorToClipboard(dpy, event.xbutton.x, event.xbutton.y, &color);
                        SCPClose();
                        break;
                    }
                    default:
                    {
                        SCPClose();
                        break;
                    };
                }
                break;
            }
            case MotionNotify:
            {
                x = event.xmotion.x;
                y = event.xmotion.y;

                SCPGetPixelColor(dpy, x, y, &color);
                XSetForeground(dpy, gc, color.pixel);
                XFillRectangle(dpy, pixelWindow, gc, 0, 0, 100, 100);
                XMoveWindow(dpy, pixelWindow, x + 10, y + 10);

                break;
            }
            default: break;
        }

        XUngrabPointer(dpy, CurrentTime);

        XFlush(dpy);
    }
}

int
main(void)
{
    SCPInit();

    SCPCreatePixelWindow(dpy, &color);

    SCPEventLoop();

    return 0;
}
