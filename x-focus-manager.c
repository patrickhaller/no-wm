#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/cursorfont.h>

/* some clients fail to reset focus on exit
 * we watch for that, and fix it
 *
 * instead of this, we also could use Elliott Hughes' clock
 * to launch x-alt-tab
 */


void reset_focus(Display *d, XEvent *e);

static void (*handler[LASTEvent]) (Display *d, XEvent *) = {
	[UnmapNotify] = reset_focus,
	[DestroyNotify] = reset_focus
};

/// fun on down

void
reset_focus(Display *d, XEvent *e) {
	XSetInputFocus(d, PointerRoot , RevertToPointerRoot, CurrentTime);
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

