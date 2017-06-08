/*
x-window-list - simple window-list (like xwininfo -root) for no-wm
Written in 2010 by Patrick Haller

To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty. You should have
received a copy of the CC0 Public Domain Dedication along with this software.
If not, see http://creativecommons.org/publicdomain/zero/1.0/ */

// http://tronche.com/gui/x/xlib/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Config {
    _Bool needs_all; // list hidden windows too?
} Config;

void x_window_list(Config cfg, Display *dpy, Window *wins, unsigned int nwins) {
    Window *w;

    for (w = wins + nwins - 1; w >= wins; w--) {
        XWindowAttributes attr;
        XTextProperty name;
        XClassHint hint;

        if (! XGetWMHints(dpy, *w))
            continue;
        if (! XGetWindowAttributes(dpy, *w, &attr))
            continue;
        if (attr.override_redirect)
            continue;
        if (attr.map_state == IsUnviewable)
            continue;
        if (attr.map_state == IsUnmapped && !cfg.needs_all)
            continue;

        XGetClassHint(dpy, *w, &hint);
        XGetWMName(dpy, *w, &name);
        printf("0x%-12x %s   %s - %s\n",
               (unsigned int)*w,
               attr.map_state == IsUnmapped ? "U":"M",
               hint.res_name, name.value
              );
    }
}

int main(int argc, char **argv) {
    Display *dpy;
    Window root, parent, *wins;
    unsigned int nwins = 0;
    Config c = { .needs_all = 0};

    if (argc > 1) {
        c.needs_all = 1;
    }

    if ((dpy = XOpenDisplay(NULL)) == NULL)
        return 1;

    XQueryTree(dpy, DefaultRootWindow(dpy), &root, &parent, &wins, &nwins);
    if (nwins == 0)
        return 0;

    x_window_list(c, dpy, wins, nwins);
    return 0;
}
