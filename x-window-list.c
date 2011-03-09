#include <X11/Xlib.h>

// http://tronche.com/gui/x/xlib/
int main(void) {
	Display *dpy;
	Window w;
	Window *root_return;
	Window *parent_return;
	Window **children_return;
	unsigned int *nchildren_return;

	if(!(dpy = XOpenDisplay(NULL)))
		return 1;
	XQueryTree(display, DefaultRootWindow(dpy), root_return, parent_return, children_return, nchildren_return);

	for (int i=0; i < nchildren_return; i++) {

