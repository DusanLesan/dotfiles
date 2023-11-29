#include <X11/Xlib.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc != 2)
		return 1;

	Display *display = XOpenDisplay(NULL);
	if (display == NULL)
		return 1;

	Window root = DefaultRootWindow(display);

	XColor color;
	if (!XAllocNamedColor(display,
				DefaultColormap(display, DefaultScreen(display)),
				argv[1], &color, &color)) {
		XCloseDisplay(display);
		return 1;
	}

	XSetWindowAttributes attributes;
	attributes.background_pixel = color.pixel;
	XChangeWindowAttributes(display, root, CWBackPixel, &attributes);
	XClearWindow(display, root);
	XFlush(display);

	XCloseDisplay(display);
	return 0;
}