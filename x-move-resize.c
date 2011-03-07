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

/* interruptibleXNextEvent() is taken from the Blender source and comes with
 * the following copyright notice: */

/* Copyright (c) Mark J. Kilgard, 1994, 1995, 1996. */

/* This program is freely distributable without licensing fees
 * and is provided without guarantee or warrantee expressed or
 * implied. This program is -not- in the public domain. */

/* Unlike XNextEvent, if a signal arrives, interruptibleXNextEvent will
 * return zero. */

static int interruptibleXNextEvent(Display *dpy, XEvent *event) {
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
			/*	LOG_ERROR("interruptibleXNextEvent(): select()\n"); */
		}
	}
}

int die(Display *dpy, Cursor *cursor, int return_value) {
	XUngrabPointer(dpy, CurrentTime);
	XFreeCursor (dpy, *cursor);
	XSync (dpy, DefaultRootWindow(dpy));
	XCloseDisplay(dpy);
	return return_value;
}

int main()
{
	Display * dpy;
	Window root;
	XWindowAttributes attr;
	XButtonEvent start;
	XEvent ev;
	Cursor cursor;
	int cursor_shape = XC_plus;
	start.button = 0; start.x_root = 0; start.y_root = 0;

	if(!(dpy = XOpenDisplay(0x0))) return 1;

	root = DefaultRootWindow(dpy);
	cursor = XCreateFontCursor(dpy, cursor_shape);
	if (XGrabPointer (dpy, root, False, (ButtonPressMask),
			GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime) != GrabSuccess)
		return die(dpy, &cursor, 1);

	for(;;) {
		if (! interruptibleXNextEvent(dpy, &ev))
			continue;
		switch(ev.type) {
			case ButtonPress:
				if (ev.xbutton.subwindow == None)
					break;
				XUngrabPointer(dpy, root);
				XSync(dpy, root);
				if (XGrabPointer (dpy, ev.xbutton.subwindow, False, (PointerMotionMask | ButtonReleaseMask),
						GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime) != GrabSuccess)
					return die(dpy, &cursor, 1);
				XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
				start = ev.xbutton;
				break;
			case MotionNotify: {
				int xdiff, ydiff;
				while (XCheckTypedEvent(dpy, MotionNotify, &ev));
				xdiff = ev.xbutton.x_root - start.x_root;
				ydiff = ev.xbutton.y_root - start.y_root;
				XMoveResizeWindow(dpy, ev.xmotion.window,
					attr.x + (start.button==1 ? xdiff : 0),
					attr.y + (start.button==1 ? ydiff : 0),
					MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
					MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
				}
				break;
			case ButtonRelease:
				return die(dpy, &cursor, 0);
				break;
		}
	}
}
