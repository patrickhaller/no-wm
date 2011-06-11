#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

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

void set_atoms(Display *dpy) { // from scrotwm via euclid-wm
	int root = DefaultRootWindow(dpy);
	Atom wm_change_state = XInternAtom(dpy,"_NET_WM_STATE",False);
	Atom wm_supported = XInternAtom(dpy,"_NET_SUPPORTED",False);
	Atom wm_check = XInternAtom(dpy,"_NET_SUPPORTING_WM_CHECK",False);
	Atom wm_name = XInternAtom(dpy,"_NET_WM_NAME",False);
	Atom utf8 = XInternAtom(dpy,"UTF8_STRING",False);
	Atom supported[] = {wm_supported, wm_name, wm_change_state};
	XChangeProperty(dpy,root,wm_check,XA_WINDOW,32,PropModeReplace,(unsigned char *)&root,1);
	XChangeProperty(dpy,root,wm_name,utf8,8,PropModeReplace,(unsigned char *) "LG3D",5);
	XChangeProperty(dpy,root,wm_supported,XA_ATOM,32,PropModeReplace,(unsigned char *) supported,4);
	XSync(dpy,False);
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
	set_atoms(dpy);

	for(;;) {
		XNextEvent(dpy, &e);
		if(handler[e.type])
			handler[e.type](dpy, &e);
	}
}

