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

#ifndef SCP_H
#define SCP_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

// SCP Global Variables
extern Display *dpy;

extern int screen;
extern int x;
extern int y;

extern Window root;
extern Window pixelWindow;

extern XEvent event;

extern XColor color;

extern Cursor cursor;

extern char *hex;

// SCP Global Functions
void SCP_Init();
void SCP_GetPixelColor(Display *display, int x, int y, XColor *color);
void SCP_CreatePixelWindow(Display *display, XColor *color);
void SCP_ChooseFormat(const char *format);
void SCP_PrintPixelColor(Display *display, int x, int y, XColor *color);
void SCP_CopyPixelColorToClipboard(Display *display, XEvent *e, int x, int y, XColor *color);
void SCP_Close();
void SCP_Main(int argc, char *argv[]);

#endif // SCP_H
