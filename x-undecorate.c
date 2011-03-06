#include <stdlib.h>
#include <X11/Xlib.h>

// mark a window as undecorated
//   i.e. remove the borders and titlebar

int main(int argc, char **argv)
{
	Display *dpy;
	XSetWindowAttributes c_wa;

	if ( ! (dpy = XOpenDisplay(0x0) ))
		return -1;

	XChangeWindowAttributes(dpy, atoi(argv[1]), CWOverrideRedirect, &c_wa);
	return 0;
}

