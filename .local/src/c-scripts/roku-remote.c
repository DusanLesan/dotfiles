#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <ctype.h>

const char *roku_ip = "192.168.56.77";
Window target_win = 0;
int should_send_notifications = 0;

void notify_event(const char *event_type, const char *key) {
	char command[256];
	snprintf(command, sizeof(command), "notify-send 'Roku Input' '%s: %s'", event_type, key);
	system(command);
}

const char* get_roku_key(KeySym keysym, XKeyEvent *ev) {
	switch (keysym) {
		case XK_Return:    return "select";
		case XK_Home:      return "home";
		case XK_Left:      return "left";
		case XK_Right:     return "right";
		case XK_Up:        return "up";
		case XK_Down:      return "down";
		case XK_F1:        return "search";
		case XK_less:      return "rev";
		case XK_greater:   return "fwd";
		case XK_BackSpace: return "back";
		case XK_F12:       return "backspace";
		default:
			break;
	}

	static char litkey[32];
	char buf[8] = {0};
	KeySym dummy;
	int len = XLookupString(ev, buf, sizeof(buf), &dummy, NULL);

	if (len == 1 && isprint(buf[0])) {
		snprintf(litkey, sizeof(litkey), "Lit_%%%02X", (unsigned char)buf[0]);
		return litkey;
	}

	return NULL;
}

int send_roku_request(KeySym keysym, XKeyEvent *ev, const char *action) {
	const char *key = get_roku_key(keysym, ev);
	if (!key) return 0;

	if (should_send_notifications)
		notify_event(action, key);

	CURL *curl;
	CURLcode res;
	int success = 1;

	char url[256];
	snprintf(url, sizeof(url), "http://%s:8060/%s/%s", roku_ip, action, key);

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) success = 0;

		curl_easy_cleanup(curl);
	}

	return success;
}

Window select_window(Display *dpy) {
	XEvent ev;
	Window root = DefaultRootWindow(dpy);

	XGrabPointer(dpy, root, False, ButtonPressMask, GrabModeSync, GrabModeAsync, None, None, CurrentTime);
	XAllowEvents(dpy, AsyncPointer, CurrentTime);
	XWindowEvent(dpy, root, ButtonPressMask, &ev);
	XUngrabPointer(dpy, CurrentTime);

	return ev.xbutton.subwindow ? ev.xbutton.subwindow : root;
}

int main(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "i:")) != -1) {
		switch (opt) {
			case 'i': roku_ip = optarg; break;
		}
	}

	Display *display = XOpenDisplay(NULL);
	if (!display) return 1;
	notify_event("Setup", "Click the window to bind Roku control");

	target_win = select_window(display);
	notify_event("Bound", "Window selected");

	Window root = DefaultRootWindow(display);
	XSelectInput(display, root, KeyPressMask | KeyReleaseMask | PointerMotionMask);

	XEvent event;
	int key_is_held = 0;
	unsigned int last_keycode = 0;
	int keyboard_grabbed = 0;

	while (1) {
		XNextEvent(display, &event);

		Window child;
		int rx, ry, wx, wy;
		unsigned int mask;
		XQueryPointer(display, root, &root, &child, &rx, &ry, &wx, &wy, &mask);

		Bool over_target = (child == target_win || root == target_win);

		if (over_target && !keyboard_grabbed) {
			if (XGrabKeyboard(display, root, True, GrabModeAsync, GrabModeAsync, CurrentTime) == GrabSuccess)
				keyboard_grabbed = 1;
		} else if (!over_target && keyboard_grabbed) {
			XUngrabKeyboard(display, CurrentTime);
			keyboard_grabbed = 0;
		}

		if (!over_target || !keyboard_grabbed) continue;

		if (event.type == KeyPress && key_is_held == 0) {
			KeySym keysym = XLookupKeysym(&event.xkey, (event.xkey.state & ShiftMask) ? 1 : 0);
			if (keysym == XK_Shift_L || keysym == XK_Shift_R) {
				continue;
			} else if (keysym == XK_Control_L || keysym == XK_Control_R) {
				continue;
			}

			key_is_held = 1;
			last_keycode = event.xkey.keycode;

			if (keysym == XK_Escape) {
				notify_event("Exiting", XKeysymToString(keysym));
				break;
			} else {
				send_roku_request(keysym,  &event.xkey, "keydown");
			}
		} else if (event.type == KeyRelease && key_is_held == 1 && event.xkey.keycode == last_keycode && XPending(display) == 0) {
			key_is_held = 0;
			KeySym keysym = XLookupKeysym(&event.xkey, 0);
			send_roku_request(keysym, &event.xkey, "keyup");
		}
	}

	XCloseDisplay(display);
	return 0;
}
