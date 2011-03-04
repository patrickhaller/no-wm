#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/cursorfont.h>

/* place all clients in the center
 */


void place_center(Display *d, XEvent *e);

static void (*handler[LASTEvent]) (Display *d, XEvent *) = {
	[MapRequest] = place_center
};

/// fun on down

void
place_center(Display *d, XEvent *e) {
	static XWindowAttributes r_wa;
	static XWindowAttributes c_wa;
	Window r = DefaultRootWindow(d);
	XMapRequestEvent *ev = &e->xmaprequest;
	int x, y;

	if(!XGetWindowAttributes(d, r, &r_wa))
		return;
	if(!XGetWindowAttributes(d, ev->window, &c_wa))
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
interruptibleXNextEvent(Display *dpy, XEvent *event) {
	fd_set fds;
	int rc;
	int dpy_fd = ConnectionNumber(dpy);
	for (;;) {
		if (XPending(dpy)) {
			XNextEvent(dpy, event);
			return 1;
		}
		FD_ZERO(&fds);
		FD_SET(dpy_fd, &fds);
		rc = select(dpy_fd + 1, &fds, NULL, NULL, NULL);
		if (rc < 0) {
			if (errno == EINTR) {
				return 0;
			}
			//	LOG_ERROR("interruptibleXNextEvent(): select()\n");
		}
	}
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
		if (! interruptibleXNextEvent(dpy, &e))
			continue;
		if(handler[e.type])
			handler[e.type](dpy, &e);
	}
}

