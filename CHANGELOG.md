# Release notes
Release notes for scolorpicker.

## v1.2.0 | 18/06/2023
**Added**
- Roundiness to the window
**Changed**
- Switched from `xclip` to `xsel` for saving to the clipboard to fix a bug
- Changed from plain old Makefile to CMake
**Fixed**
- A bug where the user couldn't close the shell when copying color to clipboard
    The bug has been fixed by changing from `xclip` (old method of saving to clipboard) to `xsel` (new method of saving to clipboard)
    Testing showed the bug has been fixed and the user can exit the shell (quit the terminal) normally without it hanging.

## v1.1.0 | 13/06/2023
**Added**
- Color code output in different modes (RGB, HSV, HSL, etc.)
    Available modes:
    - `hex` - Hexadecimal
    - `rgb` - Red, Green, Blue
- Ability to choose to output the color code to the terminal instead of the clipboard
- Amazing new CLI!

## v1.0.0 | 12/06/2023
- Inital release!
