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
