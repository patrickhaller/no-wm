/*
x-placement-manager - center misbehaving apps for no-wm
Written in 2010 by Patrick Haller

To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty. You should have
received a copy of the CC0 Public Domain Dedication along with this software.
If not, see http://creativecommons.org/publicdomain/zero/1.0/ */

#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/cursorfont.h>

/* place all clients in the center
 */

void place_center(Display *d, XEvent *e);

static void (*handler[LASTEvent]) (Display *d, XEvent *) = {
    [MapRequest] = place_center,
    [ConfigureNotify] = place_center
};

/// fun on down

void
place_center(Display *d, XEvent *e) {
    static XWindowAttributes r_wa;
    static XWindowAttributes c_wa;
    Window r = DefaultRootWindow(d);
    Window trans = None;
    XMapRequestEvent *ev = &e->xmaprequest;
    int x, y;

    if (!XGetWindowAttributes(d, r, &r_wa))
        return;
    if (!XGetWindowAttributes(d, ev->window, &c_wa))
        return;
    if (c_wa.override_redirect)
        return;
    if(XGetTransientForHint(d, ev->window, &trans))
        return;
    x = r_wa.width/2 - c_wa.width/2;
    y = r_wa.height/2 - c_wa.height/2;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    XMoveResizeWindow(d, ev->window, x, y,
                      c_wa.width > r_wa.width ? r_wa.width : c_wa.width,
                      c_wa.height > r_wa.height ? r_wa.height : c_wa.height
                     );
}

int
main(int argc, char **argv) {
    Display *dpy;
    XEvent e;
    XSetWindowAttributes wa;

    if(!(dpy = XOpenDisplay(NULL)))
        return 1;

    wa.cursor = XCreateFontCursor(dpy, XC_left_ptr);
    wa.event_mask = SubstructureNotifyMask|StructureNotifyMask;
    XChangeWindowAttributes(dpy, DefaultRootWindow(dpy), CWEventMask|CWCursor, &wa);

    for(;;) {
        XNextEvent(dpy, &e);
        if(handler[e.type])
            handler[e.type](dpy, &e);
    }
}

