/*
x-window-list - simple window-list (like xwininfo -root) for no-wm
Written in 2010 by Patrick Haller

To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty. You should have
received a copy of the CC0 Public Domain Dedication along with this software.
If not, see http://creativecommons.org/publicdomain/zero/1.0/ */


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

// http://tronche.com/gui/x/xlib/
// first window listed is top-most, last is bottom-most in window-stacking

static int cmp(Window **a, Window **b) {
	if ( **a > **b )
		return 1;
	return -1;
}

int main(int argc, char **argv) {
	Display *dpy;
	Window root, parent, *wins, *w, *winlist[256];
	unsigned int nwins, count = 0, dwin = 0;

	if (argc > 1)
		dwin = atoi(argv[1]);

	if ((dpy = XOpenDisplay(NULL)) == NULL)
		return 1;

	XQueryTree(dpy, DefaultRootWindow(dpy), &root, &parent, &wins, &nwins);

	for (w = wins + nwins - 1; w >= wins; w--) {
		XWindowAttributes attr;

		XGetWindowAttributes(dpy, *w, &attr);
		if (attr.map_state != IsViewable)
			continue;

		winlist[count] = w;
		count++;
	}
	qsort(winlist, count, sizeof(Window *), (__compar_fn_t)cmp);

	for (int i=0; i<count; i++) {
		XTextProperty name;
		XClassHint hint;
		w = winlist[i];

		XGetClassHint(dpy, *w, &hint);
		XGetWMName(dpy, *w, &name);
		printf("%02d 0x%-12x %s - %s\n", i, (unsigned int)*w, hint.res_name, name.value);
		if (argc > 1 && i == dwin) {
			XRaiseWindow(dpy, *w);
			XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
			XSync(dpy, DefaultRootWindow(dpy));
		}
	}
}
