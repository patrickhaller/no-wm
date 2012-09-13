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

// http://tronche.com/gui/x/xlib/
// first window listed is top-most, last is bottom-most in window-stacking
int main(void) {
	Display *dpy;
	Window root, parent, *wins, *w;
	unsigned int nwins;

	if ((dpy = XOpenDisplay(NULL)) == NULL)
		return 1;

	XQueryTree(dpy, DefaultRootWindow(dpy), &root, &parent, &wins, &nwins);

	for (w = wins + nwins - 1; w >= wins; w--) {
		XTextProperty name;
		XClassHint hint;
		XWindowAttributes attr;

		XGetClassHint(dpy, *w, &hint);
		XGetWMName(dpy, *w, &name);
		XGetWindowAttributes(dpy, *w, &attr);
		if (attr.map_state == IsViewable)
			printf("0x%-10x %s - %s\n", (unsigned int)*w, hint.res_name, name.value);
	}
}
