#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

const char *roku_ip = "192.168.56.77";

void notify_event(const char *event_type, const char *key) {
	char command[256];
	snprintf(command, sizeof(command), "notify-send 'Key Event' '%s: %s'", event_type, key);
	system(command);
}

const char* get_roku_key(KeySym keysym) {
	switch (keysym) {
		case XK_Return:    return "select";
		case XK_Home:      return "home";
		case XK_Left:      return "left";
		case XK_Right:     return "right";
		case XK_Up:        return "up";
		case XK_Down:      return "down";
		case XK_BackSpace: return "back";
		case XK_s:         return "search";
		case XK_less:      return "rev";
		case XK_greater:   return "fwd";
		default:           return NULL;
	}
}

int send_roku_request(KeySym keysym, const char *action) {
	const char *key = get_roku_key(keysym);

	if (!key) return 0;

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

		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			success = 0;
		}

		curl_easy_cleanup(curl);
	}

	return success;
}

int main() {
	Display *display = XOpenDisplay(NULL);
	if (!display) {
		fprintf(stderr, "Cannot open display\n");
		return 1;
	}

	if (XGrabKeyboard(display, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync, CurrentTime) != GrabSuccess) {
		fprintf(stderr, "Failed to grab keyboard\n");
		return 1;
	}

	XEvent event;
	int key_is_held = 0;
	unsigned int last_keycode = 0;

	while (1) {
		XNextEvent(display, &event);

		if (event.type == KeyPress && key_is_held == 0) {
			key_is_held = 1;
			last_keycode = event.xkey.keycode;
			KeySym keysym = XLookupKeysym(&event.xkey, 0);

			if (keysym == XK_q || keysym == XK_Escape) {
				notify_event("Exiting", XKeysymToString(keysym));
				break;
			} else {
				send_roku_request(keysym, "keydown");
			}

		} else if (event.type == KeyRelease && key_is_held == 1 && event.xkey.keycode == last_keycode && XPending(display) == 0) {
			key_is_held = 0;
			KeySym keysym = XLookupKeysym(&event.xkey, 0);
			send_roku_request(keysym, "keyup");
		}
	}

	XCloseDisplay(display);
	return 0;
}
