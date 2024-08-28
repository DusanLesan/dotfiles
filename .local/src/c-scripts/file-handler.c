#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>
#include <string.h>

static char *url_decode(const char *str) {
	char *decoded = strdup(str);
	char *p = decoded;
	while (*str) {
		if (*str == '%') {
			if (str[1] && str[2]) {
				int value;
				sscanf(str + 1, "%2x", &value);
				*p++ = (char)value;
				str += 2;
			}
		} else if (*str == '+') {
			*p++ = ' ';
		} else {
			*p++ = *str;
		}
		str++;
	}
	*p = '\0';

	return decoded;
}

static void show_items(DBusMessage *message) {
	const char *term = getenv("TERMINAL");
	DBusMessageIter iter;
	dbus_message_iter_init(message, &iter);
	DBusMessageIter array;
	dbus_message_iter_recurse(&iter, &array);
	while (dbus_message_iter_get_arg_type(&array) != DBUS_TYPE_INVALID) {
		char *item;
		dbus_message_iter_get_basic(&array, &item);
		item += 7;
		item = url_decode(item);

		char *cmd;
		asprintf(&cmd, "%s lf '%s' &", term, item);
		system(cmd);
		free(cmd);
		free(item);

		dbus_message_iter_next(&array);
	}
}

static DBusHandlerResult send_response(DBusConnection *connection, DBusMessage *message) {
	DBusMessage *reply = dbus_message_new_method_return(message);
	if (!reply) {
		fprintf(stderr, "Error creating reply message\n");
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	dbus_connection_send(connection, reply, NULL);
	dbus_message_unref(reply);

	return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult handle_get_all(DBusMessage *message, DBusConnection *connection) {
	DBusMessageIter iter;
	dbus_message_iter_init(message, &iter);
	const char *path = NULL;
	while (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_INVALID) {
		if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING) {
			dbus_message_iter_get_basic(&iter, &path);
			if (path && strcmp(path, "org.freedesktop.FileManager1") == 0) {
				return send_response(connection, message);
			}
		}
		dbus_message_iter_next(&iter);
	}

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static DBusHandlerResult message_handler(DBusConnection *connection, DBusMessage *message, void *user_data) {
	if (dbus_message_is_method_call(message, "org.freedesktop.FileManager1", "ShowItems")) {
		show_items(message);
		return send_response(connection, message);
	} else if (dbus_message_is_method_call(message, "org.freedesktop.DBus.Properties", "GetAll")) {
		return handle_get_all(message, connection);
	} else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
}

int main() {
	DBusConnection *connection = dbus_bus_get(DBUS_BUS_SESSION, NULL);
	if (connection == NULL) {
		fprintf(stderr, "Failed to connect to the D-Bus session bus\n");
		return 1;
	}

	dbus_bus_request_name(connection, "org.freedesktop.FileManager1", DBUS_NAME_FLAG_REPLACE_EXISTING, NULL);

	dbus_connection_add_filter(connection, message_handler, NULL, NULL);
	while (dbus_connection_read_write_dispatch(connection, -1));

	return 0;
}
