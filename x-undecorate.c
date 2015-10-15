/*
x-undecorate - set window as undecorated for no-wm
Written in 2010 by Patrick Haller

To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty. You should have
received a copy of the CC0 Public Domain Dedication along with this software.
If not, see http://creativecommons.org/publicdomain/zero/1.0/ */


#include <stdlib.h>
#include <X11/Xlib.h>

// mark a window as undecorated
//   i.e. remove the borders and titlebar

int main(int argc, char **argv) {
    Display *dpy;
    XSetWindowAttributes c_wa;

    if ( ! (dpy = XOpenDisplay(0x0) ))
        return -1;

    XChangeWindowAttributes(dpy, atoi(argv[1]), CWOverrideRedirect, &c_wa);
    return 0;
}

