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
    _Bool is_switch; // list windows or switch to dst window?
    unsigned int dst;
} Config;

/* static int cmp(Window **a, Window **b) {
	if ( **a > **b )
		return 1;
	return -1;
} */

void x_window_list(Config cfg, Display *dpy, Window *wins, unsigned int nwins) {
    Window *viewables[nwins], *w;
    unsigned int vc = 0;

    // get list of viewable windows
    for (w = wins + nwins - 1; w >= wins; w--) {
        XWindowAttributes attr;

        XGetWindowAttributes(dpy, *w, &attr);
        if (attr.map_state == IsViewable) {
            viewables[vc++] = w;
        }
    }
    viewables[vc] = NULL;

    // list them out
    for (int i=0; i < vc; i++) {
        XTextProperty name;
        XClassHint hint;
        w = viewables[i];

        char * res_name = XGetClassHint(dpy, *w, &hint) ? hint.res_name : "<>";
        char * wm_name  = XGetWMName(dpy, *w, &name)    ? name.value    : "<>";
        printf("%02d 0x%-12x %s - %s\n", i, (unsigned int)*w, res_name, wm_name);
        if (cfg.is_switch && i == cfg.dst) {
            XRaiseWindow(dpy, *w);
            XSetInputFocus(dpy, *w, RevertToPointerRoot, CurrentTime);
        }
    }
}

int main(int argc, char **argv) {
    Display *dpy;
    Window root, parent, *wins;
    unsigned int nwins = 0;
    Config c = { .is_switch = 0, .dst = 0 };

    if (argc > 1) {
        c.is_switch = 1;
        c.dst = atoi(argv[1]);
    }

    if ((dpy = XOpenDisplay(NULL)) == NULL)
        return 1;

    XQueryTree(dpy, DefaultRootWindow(dpy), &root, &parent, &wins, &nwins);
    if (nwins == 0)
        return 0;

    x_window_list(c, dpy, wins, nwins);
    return 0;
}
