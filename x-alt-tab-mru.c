/*
x-alt-tab-mru - alt-tab clone for no-wm
Written in 2017 by Patrick Haller

Restack window according to MRU, i.e. swap top-most with 2nd top-most.
*/


#include <X11/Xlib.h>
#include <X11/Xutil.h>


void x_alt_tab(Display *dpy, Window *wins, unsigned int nwins) {
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

    wins[nwins - 1] = *viewables[vc - 2];
    wins[nwins - 2] = *viewables[vc - 1];

    XLowerWindow(dpy, *(viewables[vc - 1]));
    w = viewables[vc - 2];
    XRaiseWindow(dpy, *w);
    XSetInputFocus(dpy, *w, RevertToPointerRoot, CurrentTime);

    XRestackWindows(dpy, wins, vc);
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

    x_alt_tab(dpy, wins, nwins);
    return 0;
}

/* To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty. You should have
received a copy of the CC0 Public Domain Dedication along with this software.
If not, see http://creativecommons.org/publicdomain/zero/1.0/ */
