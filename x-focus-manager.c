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

