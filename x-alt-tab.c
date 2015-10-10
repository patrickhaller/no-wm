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
	Window root, parent, *wins, *w = 0;

	if ( (dpy = XOpenDisplay(NULL)) == NULL)
		return 1;

	do {
		if (argc > 1)
			XCirculateSubwindowsDown(dpy, DefaultRootWindow(dpy));
		else
			XCirculateSubwindowsUp(dpy, DefaultRootWindow(dpy));

		XSync(dpy, True);
		XQueryTree(dpy, DefaultRootWindow(dpy), &root, &parent, &wins, &nwins);
		w = wins + nwins - 1;
		XGetWindowAttributes(dpy, *w, &attr);
		count++;

	} while (attr.map_state != IsViewable && count <= nwins+1);

	XSetInputFocus(dpy, *w, RevertToParent, CurrentTime);
	XSync(dpy, True);

	return 0;
}
