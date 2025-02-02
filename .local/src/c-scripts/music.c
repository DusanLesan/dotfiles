#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *player_name;
	char *artist;
	char *title;
	int is_paused;
} PlayerInfo;

DBusConnection *dbus_connection = NULL;

void init_dbus_connection() {
	DBusError error;
	dbus_error_init(&error);

	dbus_connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if (dbus_error_is_set(&error) || !dbus_connection) {
		fprintf(stderr, "Failed to connect to D-Bus: %s\n", error.message);
		dbus_error_free(&error);
		exit(1);
	}
}

void close_dbus_connection() {
	if (dbus_connection) {
		dbus_connection_unref(dbus_connection);
		dbus_connection = NULL;
	}
}

char *get_playback_status(const char *player_name) {
	DBusMessage *message, *reply;
	DBusMessageIter args;

	message = dbus_message_new_method_call(player_name, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "Get");
	if (!message) return NULL;

	const char *interface = "org.mpris.MediaPlayer2.Player";
	const char *property = "PlaybackStatus";
	dbus_message_append_args(message, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block(dbus_connection, message, -1, NULL);
	dbus_message_unref(message);
	if (!reply) return NULL;

	char *status = NULL;
	if (dbus_message_iter_init(reply, &args) && DBUS_TYPE_VARIANT == dbus_message_iter_get_arg_type(&args)) {
		DBusMessageIter variant;
		dbus_message_iter_recurse(&args, &variant);
		if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&variant))
			dbus_message_iter_get_basic(&variant, &status);
	}

	dbus_message_unref(reply);
	return status ? strdup(status) : NULL;
}

void get_metadata(const char *player_name, PlayerInfo *info) {
	DBusMessage *message, *reply;
	DBusMessageIter args, dict, entry, variant;

	message = dbus_message_new_method_call(player_name, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "Get");
	if (!message) return;

	const char *interface = "org.mpris.MediaPlayer2.Player";
	const char *property = "Metadata";
	dbus_message_append_args(message, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block(dbus_connection, message, -1, NULL);
	dbus_message_unref(message);
	if (!reply) return;

	if (dbus_message_iter_init(reply, &args) && DBUS_TYPE_VARIANT == dbus_message_iter_get_arg_type(&args)) {
		dbus_message_iter_recurse(&args, &variant);
		if (DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&variant)) {
			dbus_message_iter_recurse(&variant, &dict);
			while (DBUS_TYPE_DICT_ENTRY == dbus_message_iter_get_arg_type(&dict)) {
				dbus_message_iter_recurse(&dict, &entry);

				char *key;
				dbus_message_iter_get_basic(&entry, &key);
				dbus_message_iter_next(&entry);

				if (strcmp(key, "xesam:artist") == 0) {
					DBusMessageIter artist_iter;
					dbus_message_iter_recurse(&entry, &artist_iter);
					if (DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&artist_iter)) {
						dbus_message_iter_recurse(&artist_iter, &variant);
						if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&variant)) {
							dbus_message_iter_get_basic(&variant, &info->artist);
							info->artist = strdup(info->artist);
						}
					}
				} else if (strcmp(key, "xesam:title") == 0) {
					dbus_message_iter_recurse(&entry, &variant);
					if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&variant)) {
						dbus_message_iter_get_basic(&variant, &info->title);
						info->title = strdup(info->title);
					}
				}
				dbus_message_iter_next(&dict);
			}
		}
	}
	dbus_message_unref(reply);
}

void get_player_info(PlayerInfo *current_info) {
	DBusMessage *message, *reply;
	DBusMessageIter args, array_iter;

	message = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames");
	if (!message) return;

	reply = dbus_connection_send_with_reply_and_block(dbus_connection, message, -1, NULL);
	dbus_message_unref(message);
	if (!reply) return;

	if (dbus_message_iter_init(reply, &args) && DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&args)) {
		dbus_message_iter_recurse(&args, &array_iter);
		while (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&array_iter)) {
			char *name;
			dbus_message_iter_get_basic(&array_iter, &name);

			if (strncmp(name, "org.mpris.MediaPlayer2.", 23) == 0) {
				char *status = get_playback_status(name);

				if (status) {
					current_info->player_name = name;
					if (strcmp(status, "Playing") == 0) {
						current_info->is_paused = 0;
						get_metadata(name, current_info);
						free(status);
						break;
					} else if (strcmp(status, "Paused") == 0) {
						current_info->is_paused = 1;
						get_metadata(name, current_info);
					}
					free(status);
				}
			}
			dbus_message_iter_next(&array_iter);
		}
	}
	dbus_message_unref(reply);
}

void send_command(const char *method, const char *player) {
	DBusMessage *in = dbus_message_new_method_call(player, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", method);
	dbus_connection_send(dbus_connection, in, NULL);
}

void handle_block_button(const char *block_button, PlayerInfo *current_info) {
	int button = atoi(block_button);
	switch (button) {
		case 1: send_command("PlayPause", current_info->player_name); current_info->is_paused = !current_info->is_paused; break;
		case 3: send_command("Stop", current_info->player_name); break;
		case 4: send_command("Previous", current_info->player_name); break;
		case 5: send_command("Next", current_info->player_name); break;
	}
}

int main() {
	init_dbus_connection();
	PlayerInfo current_info = {0};
	get_player_info(&current_info);

	const char *block_button = getenv("BLOCK_BUTTON");
	if (block_button)
		handle_block_button(block_button, &current_info);

	if (current_info.is_paused) {
		printf("\n");
	} else if (current_info.artist && current_info.title) {
		if (strstr(current_info.title, " - ")) {
			printf("%s\n", current_info.title);
		} else {
			printf("%s - %s\n", current_info.artist, current_info.title);
		}
	} else if (current_info.artist) {
		printf("%s\n", current_info.artist);
	} else if (current_info.title) {
		printf("%s\n", current_info.title);
	}

	close_dbus_connection();
	return 0;
}
