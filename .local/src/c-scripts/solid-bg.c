#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <Imlib2.h>
#include <stdio.h>
#include <stdlib.h>

Display *display;

int setRootAtoms(Pixmap pixmap) {
	Atom atom_root, atom_eroot, type;
	unsigned char *data_root, *data_eroot;
	int format;
	unsigned long length, after;

	atom_root = XInternAtom(display, "_XROOTPMAP_ID", True);
	atom_eroot = XInternAtom(display, "ESETROOT_PMAP_ID", True);

	if (atom_root != None && atom_eroot != None) {
		XGetWindowProperty(display, RootWindow(display, DefaultScreen(display)), atom_root, 0L, 1L, False, AnyPropertyType, &type, &format, &length, &after, &data_root);

		if (type == XA_PIXMAP) {
			XGetWindowProperty(display, RootWindow(display, DefaultScreen(display)), atom_eroot, 0L, 1L, False, AnyPropertyType, &type, &format, &length, &after, &data_eroot);

			if (data_root && data_eroot && type == XA_PIXMAP && *((Pixmap *) data_root) == *((Pixmap *) data_eroot))
				XKillClient(display, *((Pixmap *) data_root));
		}
	}

	atom_root = XInternAtom(display, "_XROOTPMAP_ID", False);
	atom_eroot = XInternAtom(display, "ESETROOT_PMAP_ID", False);

	if (atom_root == None || atom_eroot == None)
		return 0;

	XChangeProperty(display, RootWindow(display, DefaultScreen(display)), atom_root, XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &pixmap, 1);
	XChangeProperty(display, RootWindow(display, DefaultScreen(display)), atom_eroot, XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &pixmap, 1);

	return 1;
}

int main(int argc, char **argv) {
	Visual *vis;
	Colormap cm;
	Imlib_Image image;
	int width, height, depth;
	Pixmap pixmap;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <color>\n", argv[0]);
		exit(1);
	}

	display = XOpenDisplay(NULL);
	if (!display) {
		fprintf(stderr, "Cannot open X display!\n");
		exit(1);
	}

	int screen = DefaultScreen(display);

	Imlib_Context *context = imlib_context_new();
	imlib_context_push(context);

	imlib_context_set_display(display);
	vis = DefaultVisual(display, screen);
	cm = DefaultColormap(display, screen);
	width = DisplayWidth(display, screen);
	height = DisplayHeight(display, screen);
	depth = DefaultDepth(display, screen);
	pixmap = XCreatePixmap(display, RootWindow(display, screen), width, height, depth);

	imlib_context_set_visual(vis);
	imlib_context_set_colormap(cm);
	imlib_context_set_drawable(pixmap);
	imlib_context_set_color_range(imlib_create_color_range());

	image = imlib_create_image(width, height);
	imlib_context_set_image(image);

	imlib_context_set_color(0, 0, 0, 0);
	imlib_image_fill_rectangle(0, 0, width, height);

	XColor c;
	if (XParseColor(display, cm, argv[1], &c) == 0) {
		fprintf (stderr, "Bad color (%s)\n", argv[1]);
		exit(1);
	}
	imlib_context_set_color(c.red >> 8, c.green >> 8, c.blue >> 8, 255);
	imlib_image_fill_rectangle(0, 0, width, height);

	imlib_render_image_on_drawable(0, 0);
	imlib_free_image();
	imlib_free_color_range();

	if (setRootAtoms(pixmap) == 0)
		fprintf(stderr, "Couldn't create atoms...\n");

	XKillClient(display, AllTemporary);
	XSetCloseDownMode(display, RetainTemporary);

	XSetWindowBackgroundPixmap(display, RootWindow(display, screen), pixmap);
	XClearWindow(display, RootWindow(display, screen));

	XFlush(display);
	XSync(display, False);

	imlib_context_pop();
	imlib_context_free(context);

	XCloseDisplay(display);

	return 0;
}
