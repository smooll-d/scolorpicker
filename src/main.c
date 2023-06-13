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
int outputToTerminal = False;

Window root;
Window pixelWindow;

XEvent event;

XColor color;

Cursor cursor;

GC gc;

char *hex;
char *version = "v1.1.0";
char *format = "hex";

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
SCPChooseFormat(const char *format)
{
    if (outputToTerminal == True)
    {
        if (strcmp(format, "hex") == 0)
        { printf("#%06lX\n", color.pixel); }
        if (strcmp(format, "rgb") == 0)
        { printf("rgb(%d, %d, %d)\n", color.red >> 8, color.green >> 8, color.blue >> 8); }
    }
    if (outputToTerminal == False)
    {
        if (strcmp(format, "hex") == 0)
        { sprintf(hex, "#%06lX\n", color.pixel); }
        if (strcmp(format, "rgb") == 0)
        { sprintf(hex, "rgb(%d, %d, %d)\n", color.red >> 8, color.green >> 8, color.blue >> 8); }
    }
}

void
SCPPrintPixelColor(Display *display, int x, int y, XColor *color)
{
    SCPGetPixelColor(display, x, y, color);

    SCPChooseFormat(format);
}

void
SCPCopyPixelColorToClipboard(Display *display, int x, int y, XColor *color)
{
    SCPGetPixelColor(display, x, y, color);

    SCPChooseFormat(format);

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
SCPCLIHelp()
{
    printf("Usage: scolorpicker [-h, -v, -t, -c, -f (hex, rgb, etc.)]\n\n");
    printf("Legend:\n");
    printf("    [] - Optional\n");
    printf("    () - Required\n\n");
    printf("Available options:\n");
    printf("    -h, --help                Displays this message and exits\n");
    printf("    -v, --version             Displays version of scolorpicker and exits\n");
    printf("    -t, --output-to-terminal  Outputs color code to terminal instead of clipboard\n");
    printf("    -c, --output-to-clipboard Outputs color code to clipboard instead of terminal, [DEFAULT]\n");
    printf("    -f, --format              Color code format\n\n");
    printf("Available color code formats:\n");
    printf("    hex Hexadecimal (e.g. #FFFFFF),                                                [DEFAULT]\n");
    printf("    rgb Red, Green, Blue (e.g. rgb(255, 255, 255))\n");

    exit(0);
}

void
SCPCLIVersion()
{
    printf("scolorpicker %s - Jakub Skowron (@reallySmooll) <jakubskowron676@gmail.com>\n", version);

    exit(0);
}

void
SCPCLIHandleArguments(const char *option, const char *value)
{
    if (strcmp(option, "--help") == 0 || strcmp(option, "-h") == 0)
    { SCPCLIHelp(); }
    else if (strcmp(option, "--version") == 0 || strcmp(option, "-v") == 0)
    { SCPCLIVersion(); }
    else if (strcmp(option, "--output-to-terminal") == 0 || strcmp(option, "-t") == 0)
    { outputToTerminal = True; }
    else if (strcmp(option, "--output-to-clipboard") == 0 || strcmp(option, "-c") == 0)
    { outputToTerminal = False; }
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
SCPMain(int argc, char *argv[])
{
    SCPInit();

    SCPCreatePixelWindow(dpy, &color);

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            const char *option = argv[i];
            const char *value = NULL;

            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                value = argv[i + 1];
                i++;
            }

            SCPCLIHandleArguments(option, value);
        }
    }

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
                        if (outputToTerminal == False)
                        { SCPCopyPixelColorToClipboard(dpy, event.xbutton.x, event.xbutton.y, &color); }
                        else
                        { SCPPrintPixelColor(dpy, event.xbutton.x, event.xbutton.y, &color); }

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
main(int argc, char *argv[])
{
    SCPMain(argc, argv);

    return 0;
}
