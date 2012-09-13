/*
x-move-resize - basic window ops for no-wm
Written in 2010 by Patrick Haller

To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty. You should have
received a copy of the CC0 Public Domain Dedication along with this software.
If not, see http://creativecommons.org/publicdomain/zero/1.0/ */


/* I lifted the move-resize code from tinywm and
 * added an extra pointer grab to set the cursor
 */

/* TinyWM is written by Nick Welch <mack AT incise.org>, 2005.
 *
 * This software is in the public domain
 * and is provided AS IS, with NO WARRANTY. */

#include <errno.h>
#include <sys/select.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

Cursor cursor;
XWindowAttributes attr;
XButtonEvent start;
extern void exit(int code);

// fun on down

void
configure(Display *d, Window *w, XWindowAttributes *wa) {
	XConfigureEvent ce;

	ce.type = ConfigureNotify;
	ce.display = d;
	ce.event = *w;
	ce.window = *w;
	ce.x = wa->x;
	ce.y = wa->y;
	ce.width = wa->width;
	ce.height = wa->height;
	ce.border_width = 0;
	ce.above = None;
	ce.override_redirect = False;
	XSendEvent(d, *w, False, StructureNotifyMask, (XEvent *)&ce);
}

void die(Display *dpy, int return_value) {
	XUngrabPointer(dpy, CurrentTime);
	XFreeCursor (dpy, cursor);
	XSync (dpy, DefaultRootWindow(dpy));
	XCloseDisplay(dpy);
	exit(return_value);
}

void
buttonpress(Display *dpy, XEvent *e) {
	XEvent ev = *e;
	Window root = DefaultRootWindow(dpy);
	if (ev.xbutton.subwindow == None)
		return;
	XUngrabPointer(dpy, root);
	XSync(dpy, root);
	if (XGrabPointer (dpy, ev.xbutton.subwindow, False, (PointerMotionMask | ButtonReleaseMask),
			GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime) != GrabSuccess)
		die(dpy, 1);
	XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
	start = ev.xbutton;
}

void
motionnotify(Display *dpy, XEvent *e) {
	int xdiff, ydiff;
	XEvent ev = *e;

	while (XCheckTypedEvent(dpy, MotionNotify, &ev));
	xdiff = ev.xbutton.x_root - start.x_root;
	ydiff = ev.xbutton.y_root - start.y_root;
	XMoveResizeWindow(dpy, ev.xmotion.window,
		attr.x + (start.button==1 ? xdiff : 0),
		attr.y + (start.button==1 ? ydiff : 0),
		MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
		MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
	if (start.button == 1)
		return;
}

void
buttonrelease(Display *dpy, XEvent *e) {
	die(dpy, 0);
}

int main()
{
	Display * dpy;
	Window root;
	XEvent ev;
	void (*handler[LASTEvent]) (Display *d, XEvent *) = {
		[ButtonPress] = buttonpress,
		[ButtonRelease] = buttonrelease,
		[MotionNotify] = motionnotify,
	};
	int cursor_shape = XC_plus;
	start.button = 0; start.x_root = 0; start.y_root = 0;

	if(!(dpy = XOpenDisplay(0x0))) return 1;

	root = DefaultRootWindow(dpy);
	cursor = XCreateFontCursor(dpy, cursor_shape);
	if (XGrabPointer (dpy, root, False, (ButtonPressMask),
			GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime) != GrabSuccess)
		die(dpy, 1);

	for(;;) {
		XNextEvent(dpy, &ev);
		if(handler[ev.type])
			handler[ev.type](dpy, &ev);
	}
}
