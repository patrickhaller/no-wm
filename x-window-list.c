#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

// http://tronche.com/gui/x/xlib/
// first window listed is top-most, last is bottom-most in window-stacking
int main(void) {
	Display *dpy;
	Window root;
	Window parent;
	Window *wins;
	unsigned int nwins;

	if(!(dpy = XOpenDisplay(NULL)))
		return 1;
	XQueryTree(dpy, DefaultRootWindow(dpy), &root, &parent, &wins, &nwins);

	for (int i=nwins-1; i >= 0; i--) {
		XTextProperty name;
		XClassHint hint;
		XWindowAttributes attr;

		XGetClassHint(dpy, wins[i], &hint);
		XGetWMName(dpy, wins[i], &name);
		XGetWindowAttributes(dpy, wins[i], &attr);
		if (attr.map_state == IsViewable)
			fprintf(stderr, "0x%-10x %s - %s\n", (unsigned int)wins[i], hint.res_name, name.value);
	}
}
