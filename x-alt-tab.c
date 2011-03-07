#include <X11/Xlib.h>

int main(int argc, char **argv)
{
	Display *dpy;

	if(!(dpy = XOpenDisplay(0x0))) return 1;
	if (argc > 1)
		XCirculateSubwindowsDown(dpy, DefaultRootWindow(dpy));
	else
		XCirculateSubwindowsUp(dpy, DefaultRootWindow(dpy));
	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime); /* nuke focus back to pointer */
	XSync(dpy, True);
	return 0;
}

