#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>

const char unselected_border_color[] = "#cccccc";
const char selected_border_color[]  = "#0066ff";

typedef struct {
	unsigned long unselected_border_color;
	unsigned long selected_border_color;
} Config;

typedef struct {
	Display *display;
	int screen;
	int is_other_wm_live;
	char *argv0;
} State;

State state;
Config config;

void buttonpress(XEvent *e);

static void (*handler[LASTEvent]) (XEvent *) = {
	[ButtonPress] = buttonpress
};

/// fun on down

void
die(const char *errstr, ...) {
	va_list ap;

	fprintf(stderr, "%s: ", state.argv0);
	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

int
xerror(Display *display, XErrorEvent *ee) {
	if(ee->error_code == BadWindow
	|| (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
	|| (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
	|| (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
	|| (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
	|| (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
	|| (ee->request_code == X_GrabButton && ee->error_code == BadAccess)
	|| (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
	|| (ee->request_code == X_CopyArea && ee->error_code == BadDrawable))
		return 0;
	die("fatal error: request code=%d, error code=%d",
			ee->request_code, ee->error_code);
	return 1;
}

int
xerrorstart(Display *display, XErrorEvent *ee) {
	state.is_other_wm_live = True;
	return -1;
}

void
check_other_wm() {
	state.is_other_wm_live = False;
	XSetErrorHandler(xerrorstart);
	/* this causes an error if some other window manager is running */
	XSelectInput(state.display, DefaultRootWindow(state.display), SubstructureRedirectMask);
	XSync(state.display, False);
	if(state.is_other_wm_live)
		die("another window manager is already running");
	XSetErrorHandler(xerror);
	XSync(state.display, False);
}

unsigned long
get_color(const char *colstr) {
	Colormap cmap = DefaultColormap(state.display, state.screen);
	XColor color;

	if(!XAllocNamedColor(state.display, cmap, colstr, &color, &color))
		die("error, cannot allocate color '%s'\n", colstr);
	return color.pixel;
}

void
buttonpress(XEvent *e) {
	XButtonPressedEvent *ev = &e->xbutton;

	XSetWindowBorder(state.display, ev->window, config.selected_border_color);
	XSetInputFocus(state.display, ev->window, RevertToPointerRoot, CurrentTime);
}

/*
XSelectInput(display, w, EnterWindowMask|FocusChangeMask|PropertyChangeMask|StructureNotifyMask);
wa.event_mask = SubstructureRedirectMask|SubstructureNotifyMask|ButtonPressMask
				|EnterWindowMask|LeaveWindowMask|StructureNotifyMask
				|PropertyChangeMask;
XChangeWindowAttributes(display, root, CWEventMask|CWCursor, &wa);
XSelectInput(display, root, wa.event_mask);
*/


int
main(int argc, char **argv) {
	XEvent e;

	state.argv0 = argv[0];

	if(!(state.display = XOpenDisplay(NULL)))
		die("Could not open display!");
	state.screen = 0;
	check_other_wm();

	config.selected_border_color = get_color(selected_border_color);
	config.unselected_border_color = get_color(unselected_border_color);

	while(!XNextEvent(state.display, &e)) {
		if(handler[e.type])
			handler[e.type](&e); /* call handler */
	}

	return 0;
}

