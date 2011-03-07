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
	XMapRequestEvent *ev = &e->xmaprequest;
	int x, y;

	if (!XGetWindowAttributes(d, r, &r_wa))
		return;
	if (!XGetWindowAttributes(d, ev->window, &c_wa))
		return;
	if (c_wa.override_redirect)
		return;
	x = r_wa.width/2 - c_wa.width/2;
	y = r_wa.height/2 - c_wa.height/2;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	XMoveWindow(d, ev->window, x, y);
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
	XChangeWindowAttributes(dpy, DefaultRootWindow(dpy) , CWEventMask|CWCursor, &wa);

	for(;;) {
		XNextEvent(dpy, &e);
		if(handler[e.type])
			handler[e.type](dpy, &e);
	}
}

