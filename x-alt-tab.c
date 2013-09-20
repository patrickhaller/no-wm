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

int main(int argc, char **argv)
{
	Display *dpy;
	XWindowAttributes attr;
	unsigned int nwins, count = 0;

	if ( (dpy = XOpenDisplay(NULL)) == NULL)
		return 1;

	do {
		Window root, parent, *wins, *w;
		XTextProperty name;
		XClassHint hint;

		if (argc > 1)
			XCirculateSubwindowsDown(dpy, DefaultRootWindow(dpy));
		else
			XCirculateSubwindowsUp(dpy, DefaultRootWindow(dpy));

		XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
		XSync(dpy, True);
		XQueryTree(dpy, DefaultRootWindow(dpy), &root, &parent, &wins, &nwins);
		w = wins + nwins - 1;
		XGetClassHint(dpy, *w, &hint);
		XGetWMName(dpy, *w, &name);
		XGetWindowAttributes(dpy, *w, &attr);
		count++;
	} while (attr.width <= 1 && count <= nwins+1);

	return 0;
}
