/*
x-alt-tab - alt-tab clone for no-wm
Written in 2010 by Patrick Haller

To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty. You should have
received a copy of the CC0 Public Domain Dedication along with this software.
If not, see http://creativecommons.org/publicdomain/zero/1.0/ */

#include <X11/Xlib.h>
#include <X11/Xutil.h>

// Which way do we want to rotate the window stack?
typedef enum RotationEnum { bottom_to_top, top_to_bottom } Rotation;

void x_alt_tab(Rotation r, Display *dpy, Window *wins, unsigned int nwins) {
    Window *viewables[nwins], *w = 0;
    unsigned int vc = 0;

    // make list of viewable windows
    for (w = wins; w - wins < nwins; w++) {
        XWindowAttributes attr;

        if (! XGetWMHints(dpy, *w))
            continue;
        if (! XGetWindowAttributes(dpy, *w, &attr))
            continue;
        if (attr.override_redirect)
            continue;
        if (attr.map_state != IsViewable)
            continue;
        viewables[vc++] = w;
    }
    viewables[vc] = NULL;

    // promote the bottom to top, or demote top to bottom and raise 2nd
    if (r == bottom_to_top)
        w = viewables[0];
    else {
        XLowerWindow(dpy, *(viewables[vc - 1]));
        w = viewables[vc - 2];
    }

    XRaiseWindow(dpy, *w);
    XSetInputFocus(dpy, *w, RevertToPointerRoot, CurrentTime);
    XSync(dpy, True);
}

int main(int argc, char **argv) {
    Display *dpy;
    unsigned int nwins = 0;
    Window root, parent, *wins = 0;

    if ( (dpy = XOpenDisplay(NULL)) == NULL)
        return 1;

    XSync(dpy, True);
    XQueryTree(dpy, DefaultRootWindow(dpy), &root, &parent, &wins, &nwins);
    if (nwins <= 1)
        return 0;

    Rotation r = (argc == 1) ? bottom_to_top : top_to_bottom;
    x_alt_tab(r, dpy, wins, nwins);
    return 0;
}
