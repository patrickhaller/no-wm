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

void alt_tab(int argc, Display *dpy, Window *wins, unsigned int nwins) {
	XWindowAttributes attr;
	Window *viewables[nwins], *w = 0;
	unsigned int count = 0;
	int vc = 0;

	// make list of viewable windows
	for (count = 0; count < nwins; count++) {
		w = wins + count;
		XGetWindowAttributes(dpy, *w, &attr);
		if (attr.map_state == IsViewable) {
			viewables[vc] = w;
			vc++;
		}
	}
	viewables[vc] = NULL;

	// promote the last to top, or demote top to last and raise 2nd
	if (argc > 1) {
		w = viewables[vc - 2];
		XLowerWindow(dpy, *(viewables[vc - 1]));
	} else {
		w = viewables[0];
	}

	XRaiseWindow(dpy, *w);
	XSetInputFocus(dpy, *w, RevertToPointerRoot, CurrentTime);
	XSync(dpy, True);
}

int main(int argc, char **argv)
{
	Display *dpy;
	unsigned int nwins = 0;
	Window root, parent, *wins = 0;

	if ( (dpy = XOpenDisplay(NULL)) == NULL)
		return 1;

	XSync(dpy, True);
	XQueryTree(dpy, DefaultRootWindow(dpy), &root, &parent, &wins, &nwins);
	if (nwins == 1)
		return 0;

	alt_tab(argc, dpy, wins, nwins);
	return 0;
}
