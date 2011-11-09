#include <X11/Xlib.h>

int main(int argc, char **argv)
{
	Display *dpy;

	if ( (dpy = XOpenDisplay(NULL)) == NULL)
		return 1;

	if (argc > 1)
		XCirculateSubwindowsDown(dpy, DefaultRootWindow(dpy));
	else
		XCirculateSubwindowsUp(dpy, DefaultRootWindow(dpy));

	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
	XSync(dpy, True);
	return 0;
}
