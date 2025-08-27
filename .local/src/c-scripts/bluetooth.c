#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include <sys/stat.h>

#define BLUEZ_SERVICE "org.bluez"
#define BLUEZ_DEVICE_INTERFACE "org.bluez.Device1"
#define BLUEZ_BATTERY_INTERFACE "org.bluez.Battery1"
#define BLUEZ_GATT_CHARACTERISTIC_INTERFACE "org.bluez.GattCharacteristic1"
#define DBUS_OBJECT_MANAGER_INTERFACE "org.freedesktop.DBus.ObjectManager"
#define BATTERY_LEVEL_UUID "00002A19-0000-1000-8000-00805F9B34FB"
#define BATTERY_SERVICE_UUID "0000180F-0000-1000-8000-00805F9B34FB"
#define BATTERY_USER_DESC_UUID "00002901-0000-1000-8000-00805F9B34FB"

#define CACHE_FILE_PATH "/tmp/batterycache"
#define DEFAULT_CACHE_TIMEOUT 90
#define MAX_CACHED_DEVICES 32
#define MAX_GATT_BATTERIES 8
#define MAX_DESCRIPTION_LENGTH 64
#define MAX_PATH_LENGTH 128

typedef struct {
	bool use_gatt;
	bool verbose_output;
	int cache_timeout;
} Config;

typedef struct {
	int level;
	char *description;
	char *object_path;
} BatteryInfo;

typedef struct {
	char *path;
	char *alias;
	char *address;
	int standard_battery_level;
	bool has_standard_battery;
	BatteryInfo *gatt_batteries;
	int gatt_battery_count;
} BluetoothDevice;

typedef struct {
	BluetoothDevice *devices;
	int count;
	int capacity;
} DeviceList;

typedef struct {
	char address[18];
	int gatt_battery_count;
	int gatt_battery_levels[MAX_GATT_BATTERIES];
	char gatt_battery_descriptions[MAX_GATT_BATTERIES][MAX_DESCRIPTION_LENGTH];
	char gatt_battery_paths[MAX_GATT_BATTERIES][MAX_PATH_LENGTH];
} CachedDeviceInfo;

typedef struct {
	int device_count;
	time_t timestamp;
	CachedDeviceInfo devices[MAX_CACHED_DEVICES];
} BatteryCache;

static void str_to_upper(char *str) {
	if (!str) return;
	for (; *str; str++) {
		if (*str >= 'a' && *str <= 'z') {
			*str = *str - 'a' + 'A';
		}
	}
}

static BluetoothDevice* add_device(DeviceList *list, const char *path) {
	if (list->count >= list->capacity) {
		list->capacity = list->capacity ? list->capacity * 2 : 8;
		list->devices = realloc(list->devices, sizeof(BluetoothDevice) * list->capacity);
	}

	BluetoothDevice *dev = &list->devices[list->count++];
	memset(dev, 0, sizeof(BluetoothDevice));
	dev->path = path ? strdup(path) : NULL;
	dev->standard_battery_level = -1;
	return dev;
}

static void add_gatt_battery(BluetoothDevice *device, int level, const char *description, const char *object_path) {
	device->gatt_batteries = realloc(device->gatt_batteries,
			sizeof(BatteryInfo) * (device->gatt_battery_count + 1));

	BatteryInfo *battery = &device->gatt_batteries[device->gatt_battery_count++];
	battery->level = level;
	battery->description = description ? strdup(description) : strdup("GATT Battery");
	battery->object_path = object_path ? strdup(object_path) : NULL;
}

static void free_device_list(DeviceList *list) {
	for (int i = 0; i < list->count; i++) {
		free(list->devices[i].path);
		free(list->devices[i].alias);
		free(list->devices[i].address);

		for (int j = 0; j < list->devices[i].gatt_battery_count; j++) {
			free(list->devices[i].gatt_batteries[j].description);
			free(list->devices[i].gatt_batteries[j].object_path);
		}
		free(list->devices[i].gatt_batteries);
	}
	free(list->devices);
	memset(list, 0, sizeof(DeviceList));
}

static char* extract_string(DBusMessageIter *variant) {
	if (dbus_message_iter_get_arg_type(variant) != DBUS_TYPE_STRING) return NULL;
	char *str;
	dbus_message_iter_get_basic(variant, &str);
	return str ? strdup(str) : NULL;
}

static bool extract_bool(DBusMessageIter *variant) {
	if (dbus_message_iter_get_arg_type(variant) != DBUS_TYPE_BOOLEAN) return false;
	dbus_bool_t val;
	dbus_message_iter_get_basic(variant, &val);
	return val;
}

static int extract_byte(DBusMessageIter *variant) {
	if (dbus_message_iter_get_arg_type(variant) != DBUS_TYPE_BYTE) return -1;
	unsigned char val;
	dbus_message_iter_get_basic(variant, &val);
	return val;
}

static int compare_battery_by_path(const void *a, const void *b) {
	const BatteryInfo *battery_a = (const BatteryInfo *)a;
	const BatteryInfo *battery_b = (const BatteryInfo *)b;

	if (battery_a->object_path && battery_b->object_path)
		return strcmp(battery_a->object_path, battery_b->object_path);

	return strcmp(battery_a->description, battery_b->description);
}

static void sort_device_batteries_by_path(BluetoothDevice *device) {
	if (device->gatt_battery_count > 1)
		qsort(device->gatt_batteries, device->gatt_battery_count,
				sizeof(BatteryInfo), compare_battery_by_path);
}

static bool load_battery_cache(BatteryCache *cache) {
	FILE *f = fopen(CACHE_FILE_PATH, "rb");
	if (!f) return false;

	size_t read = fread(cache, sizeof(BatteryCache), 1, f);
	fclose(f);

	return read == 1;
}

static bool save_battery_cache(const BatteryCache *cache) {
	FILE *f = fopen(CACHE_FILE_PATH, "wb");
	if (!f) {
		fprintf(stderr, "Warning: Could not open cache file for writing\n");
		return false;
	}

	size_t written = fwrite(cache, sizeof(BatteryCache), 1, f);
	fclose(f);

	if (written != 1) {
		fprintf(stderr, "Warning: Could not write to cache file\n");
		return false;
	}

	return true;
}

static void populate_from_cache(DeviceList *devices, const BatteryCache *cache) {
	for (int i = 0; i < devices->count; i++) {
		BluetoothDevice *dev = &devices->devices[i];
		if (!dev->address) continue;

		for (int j = 0; j < cache->device_count; j++) {
			if (strcmp(dev->address, cache->devices[j].address) == 0) {
				for (int k = 0; k < cache->devices[j].gatt_battery_count; k++) {
					int level = cache->devices[j].gatt_battery_levels[k];
					const char *desc = cache->devices[j].gatt_battery_descriptions[k];
					const char *path = cache->devices[j].gatt_battery_paths[k];

					if (level >= 0) {
						add_gatt_battery(dev, level, desc, path);
					}
				}
				sort_device_batteries_by_path(dev);
				break;
			}
		}
	}
}

static void update_cache_from_devices(BatteryCache *cache, const DeviceList *devices) {
	cache->timestamp = time(NULL);
	cache->device_count = 0;

	for (int i = 0; i < devices->count && cache->device_count < MAX_CACHED_DEVICES; i++) {
		const BluetoothDevice *dev = &devices->devices[i];
		if (!dev->address) continue;

		CachedDeviceInfo *cached_dev = &cache->devices[cache->device_count++];
		strncpy(cached_dev->address, dev->address, sizeof(cached_dev->address) - 1);
		cached_dev->address[sizeof(cached_dev->address) - 1] = '\0';

		cached_dev->gatt_battery_count = 0;

		if (dev->gatt_battery_count > 0) {
			for (int j = 0; j < dev->gatt_battery_count && j < MAX_GATT_BATTERIES; j++) {
				cached_dev->gatt_battery_levels[cached_dev->gatt_battery_count] = dev->gatt_batteries[j].level;

				strncpy(cached_dev->gatt_battery_descriptions[cached_dev->gatt_battery_count],
						dev->gatt_batteries[j].description ? dev->gatt_batteries[j].description : "GATT Battery",
						sizeof(cached_dev->gatt_battery_descriptions[cached_dev->gatt_battery_count]) - 1);
				cached_dev->gatt_battery_descriptions[cached_dev->gatt_battery_count][MAX_DESCRIPTION_LENGTH-1] = '\0';

				strncpy(cached_dev->gatt_battery_paths[cached_dev->gatt_battery_count],
						dev->gatt_batteries[j].object_path ? dev->gatt_batteries[j].object_path : "",
						sizeof(cached_dev->gatt_battery_paths[cached_dev->gatt_battery_count]) - 1);
				cached_dev->gatt_battery_paths[cached_dev->gatt_battery_count][MAX_PATH_LENGTH-1] = '\0';

				cached_dev->gatt_battery_count++;
			}
		} else {
			cached_dev->gatt_battery_levels[0] = -1;
			strncpy(cached_dev->gatt_battery_descriptions[0], "No GATT Battery",
					sizeof(cached_dev->gatt_battery_descriptions[0]) - 1);
			cached_dev->gatt_battery_descriptions[0][MAX_DESCRIPTION_LENGTH-1] = '\0';
			cached_dev->gatt_battery_paths[0][0] = '\0';
			cached_dev->gatt_battery_count = 1;
		}
	}
}

static bool device_list_changed(const DeviceList *current_devices, const BatteryCache *cache) {
	if (current_devices->count != cache->device_count) return true;

	for (int i = 0; i < current_devices->count; i++) {
		const BluetoothDevice *dev = &current_devices->devices[i];
		if (!dev->address) continue;

		bool found = false;
		for (int j = 0; j < cache->device_count; j++) {
			if (strcmp(dev->address, cache->devices[j].address) == 0) {
				found = true;
				break;
			}
		}
		if (!found) return true;
	}

	return false;
}

static DBusMessage* dbus_call_method(DBusConnection *conn, const char *service, const char *path,
		const char *interface, const char *method) {
	DBusMessage *msg = dbus_message_new_method_call(service, path, interface, method);
	if (!msg) return NULL;

	DBusError error;
	dbus_error_init(&error);
	DBusMessage *reply = dbus_connection_send_with_reply_and_block(conn, msg, 5000, &error);
	dbus_message_unref(msg);

	if (!reply) {
		dbus_error_free(&error);
		return NULL;
	}

	return reply;
}

static int read_gatt_characteristic(DBusConnection *conn, const char *char_path) {
	DBusMessage *msg = dbus_message_new_method_call(BLUEZ_SERVICE, char_path,
			BLUEZ_GATT_CHARACTERISTIC_INTERFACE,
			"ReadValue");
	if (!msg) return -1;

	DBusMessageIter args, dict;
	dbus_message_iter_init_append(msg, &args);
	dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &dict);
	dbus_message_iter_close_container(&args, &dict);

	DBusError error;
	dbus_error_init(&error);
	DBusMessage *reply = dbus_connection_send_with_reply_and_block(conn, msg, 5000, &error);
	dbus_message_unref(msg);

	if (!reply) {
		dbus_error_free(&error);
		return -1;
	}

	int value = -1;
	DBusMessageIter reply_iter, array;
	if (dbus_message_iter_init(reply, &reply_iter) &&
			dbus_message_iter_get_arg_type(&reply_iter) == DBUS_TYPE_ARRAY) {
		dbus_message_iter_recurse(&reply_iter, &array);
		if (dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_BYTE) {
			unsigned char val;
			dbus_message_iter_get_basic(&array, &val);
			value = val;
		}
	}

	dbus_message_unref(reply);
	return value;
}

static char* read_gatt_descriptor(DBusConnection *conn, const char *desc_path) {
	DBusMessage *msg = dbus_message_new_method_call(BLUEZ_SERVICE, desc_path,
			"org.bluez.GattDescriptor1",
			"ReadValue");
	if (!msg) return NULL;

	DBusMessageIter args, dict;
	dbus_message_iter_init_append(msg, &args);
	dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &dict);
	dbus_message_iter_close_container(&args, &dict);

	DBusError error;
	dbus_error_init(&error);
	DBusMessage *reply = dbus_connection_send_with_reply_and_block(conn, msg, 5000, &error);
	dbus_message_unref(msg);

	if (!reply) {
		dbus_error_free(&error);
		return NULL;
	}

	char *result = NULL;
	DBusMessageIter reply_iter, array;
	if (dbus_message_iter_init(reply, &reply_iter) &&
			dbus_message_iter_get_arg_type(&reply_iter) == DBUS_TYPE_ARRAY) {
		dbus_message_iter_recurse(&reply_iter, &array);

		int byte_count = 0;
		char buffer[256] = {0};

		while (dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_BYTE && byte_count < 255) {
			unsigned char val;
			dbus_message_iter_get_basic(&array, &val);
			buffer[byte_count++] = val;
			dbus_message_iter_next(&array);
		}

		if (byte_count > 0) {
			buffer[byte_count] = '\0';
			result = strdup(buffer);
		}
	}

	dbus_message_unref(reply);
	return result;
}

static char* get_battery_name_from_descriptors(DBusConnection *conn, const char *char_path) {
	DBusMessage *reply = dbus_call_method(conn, BLUEZ_SERVICE, "/",
			DBUS_OBJECT_MANAGER_INTERFACE, "GetManagedObjects");
	if (!reply) return NULL;

	char *battery_name = NULL;
	DBusMessageIter iter, objects;
	dbus_message_iter_init(reply, &iter);
	dbus_message_iter_recurse(&iter, &objects);

	while (dbus_message_iter_get_arg_type(&objects) == DBUS_TYPE_DICT_ENTRY) {
		DBusMessageIter object_entry, interfaces;
		char *object_path;

		dbus_message_iter_recurse(&objects, &object_entry);
		dbus_message_iter_get_basic(&object_entry, &object_path);

		if (strstr(object_path, char_path)) {
			dbus_message_iter_next(&object_entry);
			dbus_message_iter_recurse(&object_entry, &interfaces);

			while (dbus_message_iter_get_arg_type(&interfaces) == DBUS_TYPE_DICT_ENTRY) {
				DBusMessageIter interface_entry, properties;
				char *interface_name;

				dbus_message_iter_recurse(&interfaces, &interface_entry);
				dbus_message_iter_get_basic(&interface_entry, &interface_name);

				if (strcmp(interface_name, "org.bluez.GattDescriptor1") == 0) {
					dbus_message_iter_next(&interface_entry);
					dbus_message_iter_recurse(&interface_entry, &properties);

					char *uuid = NULL;
					while (dbus_message_iter_get_arg_type(&properties) == DBUS_TYPE_DICT_ENTRY) {
						DBusMessageIter property_entry, variant;
						char *property_name;

						dbus_message_iter_recurse(&properties, &property_entry);
						dbus_message_iter_get_basic(&property_entry, &property_name);
						dbus_message_iter_next(&property_entry);
						dbus_message_iter_recurse(&property_entry, &variant);

						if (strcmp(property_name, "UUID") == 0) {
							uuid = extract_string(&variant);
							break;
						}
						dbus_message_iter_next(&properties);
					}

					if (uuid) {
						str_to_upper(uuid);
						if (strcmp(uuid, BATTERY_USER_DESC_UUID) == 0) {
							battery_name = read_gatt_descriptor(conn, object_path);
							free(uuid);
							break;
						}
						free(uuid);
					}
				}
				dbus_message_iter_next(&interfaces);
			}

			if (battery_name) break;
		}
		dbus_message_iter_next(&objects);
	}

	dbus_message_unref(reply);
	return battery_name;
}

static void scan_gatt_batteries(DBusConnection *conn, DeviceList *devices) {
	DBusMessage *reply = dbus_call_method(conn, BLUEZ_SERVICE, "/",
			DBUS_OBJECT_MANAGER_INTERFACE, "GetManagedObjects");
	if (!reply) return;

	DBusMessageIter iter, objects;
	dbus_message_iter_init(reply, &iter);
	dbus_message_iter_recurse(&iter, &objects);

	while (dbus_message_iter_get_arg_type(&objects) == DBUS_TYPE_DICT_ENTRY) {
		DBusMessageIter object_entry, interfaces;
		char *object_path;

		dbus_message_iter_recurse(&objects, &object_entry);
		dbus_message_iter_get_basic(&object_entry, &object_path);
		dbus_message_iter_next(&object_entry);
		dbus_message_iter_recurse(&object_entry, &interfaces);

		while (dbus_message_iter_get_arg_type(&interfaces) == DBUS_TYPE_DICT_ENTRY) {
			DBusMessageIter interface_entry, properties;
			char *interface_name;

			dbus_message_iter_recurse(&interfaces, &interface_entry);
			dbus_message_iter_get_basic(&interface_entry, &interface_name);

			if (strcmp(interface_name, BLUEZ_GATT_CHARACTERISTIC_INTERFACE) == 0) {
				dbus_message_iter_next(&interface_entry);
				dbus_message_iter_recurse(&interface_entry, &properties);

				char *uuid = NULL;
				while (dbus_message_iter_get_arg_type(&properties) == DBUS_TYPE_DICT_ENTRY) {
					DBusMessageIter property_entry, variant;
					char *property_name;

					dbus_message_iter_recurse(&properties, &property_entry);
					dbus_message_iter_get_basic(&property_entry, &property_name);
					dbus_message_iter_next(&property_entry);
					dbus_message_iter_recurse(&property_entry, &variant);

					if (strcmp(property_name, "UUID") == 0) {
						uuid = extract_string(&variant);
						break;
					}
					dbus_message_iter_next(&properties);
				}

				if (uuid) {
					str_to_upper(uuid);
					if (strcmp(uuid, BATTERY_LEVEL_UUID) == 0) {
						for (int i = 0; i < devices->count; i++) {
							if (strstr(object_path, devices->devices[i].path)) {
								int level = read_gatt_characteristic(conn, object_path);
								if (level >= 0) {
									char *peripheral_name = get_battery_name_from_descriptors(conn, object_path);
									if (!peripheral_name)
										peripheral_name = strdup("GATT Battery");

									add_gatt_battery(&devices->devices[i], level, peripheral_name, object_path);
									free(peripheral_name);
								}
								break;
							}
						}
					}
					free(uuid);
				}
			}
			dbus_message_iter_next(&interfaces);
		}
		dbus_message_iter_next(&objects);
	}

	dbus_message_unref(reply);

	for (int i = 0; i < devices->count; i++) {
		sort_device_batteries_by_path(&devices->devices[i]);
	}
}

static void scan_bluetooth_devices(DBusConnection *conn, DeviceList *devices, const Config *config) {
	DBusMessage *reply = dbus_call_method(conn, BLUEZ_SERVICE, "/",
			DBUS_OBJECT_MANAGER_INTERFACE, "GetManagedObjects");
	if (!reply) {
		fprintf(stderr, "Failed to get managed objects\n");
		return;
	}

	DBusMessageIter iter, objects;
	dbus_message_iter_init(reply, &iter);
	dbus_message_iter_recurse(&iter, &objects);

	while (dbus_message_iter_get_arg_type(&objects) == DBUS_TYPE_DICT_ENTRY) {
		DBusMessageIter object_entry, interfaces;
		char *object_path;

		dbus_message_iter_recurse(&objects, &object_entry);
		dbus_message_iter_get_basic(&object_entry, &object_path);
		dbus_message_iter_next(&object_entry);
		dbus_message_iter_recurse(&object_entry, &interfaces);

		BluetoothDevice *current_device = NULL;

		while (dbus_message_iter_get_arg_type(&interfaces) == DBUS_TYPE_DICT_ENTRY) {
			DBusMessageIter interface_entry, properties;
			char *interface_name;

			dbus_message_iter_recurse(&interfaces, &interface_entry);
			dbus_message_iter_get_basic(&interface_entry, &interface_name);

			if (strcmp(interface_name, BLUEZ_DEVICE_INTERFACE) == 0) {
				dbus_message_iter_next(&interface_entry);
				dbus_message_iter_recurse(&interface_entry, &properties);

				bool connected = false;
				char *alias = NULL;
				char *address = NULL;

				while (dbus_message_iter_get_arg_type(&properties) == DBUS_TYPE_DICT_ENTRY) {
					DBusMessageIter property_entry, variant;
					char *property_name;

					dbus_message_iter_recurse(&properties, &property_entry);
					dbus_message_iter_get_basic(&property_entry, &property_name);
					dbus_message_iter_next(&property_entry);
					dbus_message_iter_recurse(&property_entry, &variant);

					if (strcmp(property_name, "Connected") == 0) {
						connected = extract_bool(&variant);
					} else if (strcmp(property_name, "Alias") == 0) {
						alias = extract_string(&variant);
					} else if (strcmp(property_name, "Address") == 0) {
						address = extract_string(&variant);
					}

					dbus_message_iter_next(&properties);
				}

				if (connected) {
					current_device = add_device(devices, object_path);
					current_device->alias = alias;
					current_device->address = address;
				} else {
					free(alias);
					free(address);
				}
			}
			else if (strcmp(interface_name, BLUEZ_BATTERY_INTERFACE) == 0 && current_device) {
				current_device->has_standard_battery = true;

				dbus_message_iter_next(&interface_entry);
				dbus_message_iter_recurse(&interface_entry, &properties);

				while (dbus_message_iter_get_arg_type(&properties) == DBUS_TYPE_DICT_ENTRY) {
					DBusMessageIter property_entry, variant;
					char *property_name;

					dbus_message_iter_recurse(&properties, &property_entry);
					dbus_message_iter_get_basic(&property_entry, &property_name);
					dbus_message_iter_next(&property_entry);
					dbus_message_iter_recurse(&property_entry, &variant);

					if (strcmp(property_name, "Percentage") == 0)
						current_device->standard_battery_level = extract_byte(&variant);

					dbus_message_iter_next(&properties);
				}
			}
			dbus_message_iter_next(&interfaces);
		}
		dbus_message_iter_next(&objects);
	}

	dbus_message_unref(reply);

	if (config->use_gatt) {
		BatteryCache cache = {0};
		struct stat st;
		bool cache_valid = (stat(CACHE_FILE_PATH, &st) == 0) &&
			((time(NULL) - st.st_mtime) < config->cache_timeout) &&
			load_battery_cache(&cache);
		bool devices_changed = false;

		if (cache_valid)
			devices_changed = device_list_changed(devices, &cache);

		if (cache_valid && !devices_changed) {
			if (config->verbose_output)
				printf("Using cached GATT battery data (age: %ld seconds)\n", time(NULL) - cache.timestamp);
			populate_from_cache(devices, &cache);
		} else {
			if (config->verbose_output) {
				if (!cache_valid) {
					printf("Cache invalid or missing, scanning GATT batteries...\n");
				} else {
					printf("Device list changed, rescanning GATT batteries...\n");
				}
			}

			scan_gatt_batteries(conn, devices);
			update_cache_from_devices(&cache, devices);
			save_battery_cache(&cache);
		}
	}
}

static void print_verbose_output(const DeviceList *devices) {
	printf("\nBluetooth Devices:\n");
	printf("==================\n\n");
	if (devices->count == 0) {
		printf("No connected Bluetooth devices found.\n");
		return;
	}
	for (int i = 0; i < devices->count; i++) {
		const BluetoothDevice *dev = &devices->devices[i];
		printf("Device: %s\n", dev->alias ? dev->alias : "Unknown");
		printf("  Address: %s\n", dev->address ? dev->address : "Unknown");
		printf("  Connected: Yes\n");
		if (dev->has_standard_battery && dev->standard_battery_level >= 0)
			printf("  Standard Battery: %d%%\n", dev->standard_battery_level);
		if (dev->gatt_battery_count > 0) {
			printf("  GATT Batteries:\n");
			for (int j = 0; j < dev->gatt_battery_count; j++) {
				printf("    %s: %d%%\n", dev->gatt_batteries[j].description, dev->gatt_batteries[j].level);
			}
		}
		printf("\n");
	}
}

static void print_short_output(const DeviceList *devices) {
	for (int i = 0; i < devices->count; i++) {
		const BluetoothDevice *dev = &devices->devices[i];
		if (!dev->alias) continue;

		printf("%s", dev->alias);

		if (dev->gatt_battery_count > 0) {
			for (int j = 0; j < dev->gatt_battery_count; j++) {
				printf("%s%d%%", j ? "; " : " ", dev->gatt_batteries[j].level);
			}
		} else if (dev->has_standard_battery && dev->standard_battery_level >= 0 && dev->standard_battery_level < 100) {
			printf(" %d%%", dev->standard_battery_level);
		}

		if (i < devices->count - 1)
			printf("  ");
		else
			printf("\n");
	}
}

static bool init_bluetooth(DBusConnection **conn) {
	DBusError err;
	dbus_error_init(&err);

	*conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (dbus_error_is_set(&err)) {
		fprintf(stderr, "D-Bus connection error: %s\n", err.message);
		dbus_error_free(&err);
		return false;
	}

	return *conn != NULL;
}

static void print_usage(const char *program_name) {
	printf("Usage: %s [options]\n", program_name);
	printf("Options:\n");
	printf("  -g              Disable GATT battery service scanning\n");
	printf("  -v              Verbose output format\n");
	printf("  -t <seconds>    Cache timeout in seconds (default: %d)\n", DEFAULT_CACHE_TIMEOUT);
	printf("  -h              Show this help message\n");
}

static bool parse_args(int argc, char *argv[], Config *config) {
	config->cache_timeout = DEFAULT_CACHE_TIMEOUT;

	int opt;
	while ((opt = getopt(argc, argv, "gvt:h")) != -1) {
		switch (opt) {
			case 'g':
				config->use_gatt = false;
				break;
			case 'v':
				config->verbose_output = true;
				break;
			case 't':
				config->cache_timeout = atoi(optarg);
				if (config->cache_timeout <= 0) {
					fprintf(stderr, "Invalid cache timeout: %s\n", optarg);
					return false;
				}
				break;
			case 'h':
				return false;
			default:
				fprintf(stderr, "Unknown option: %c\n", opt);
				print_usage(argv[0]);
				return false;
		}
	}
	return true;
}

void dmenu_bluetooth(void) {
	pid_t child_pid = fork();
	if (child_pid == 0) {
		char *args[] = {"/bin/alacritty", "--class", "floating", "-e", "dmenu-bluetooth", NULL};
		setenv("WINIT_X11_SCALE_FACTOR", "1", 1);
		setenv("USE_TERM", "true", 1);
		execv(args[0], args);
	}
}

void notification(void) {
	pid_t child_pid = fork();
	if (child_pid == 0) {
		char *args[] = {"/bin/alacritty", "--class", "floating", "--hold", "-e", "bluetooth", "-vt", "1", NULL};
		execv(args[0], args);
	}
}

int main(int argc, char *argv[]) {
	Config config = {0};
	config.use_gatt = true;
	if (!parse_args(argc, argv, &config))
		return EXIT_FAILURE;

	char* button = getenv("BLOCK_BUTTON");
	if(button != NULL) {
		switch (atoi(button)) {
			case 1:
				dmenu_bluetooth();
				break;
			case 3:
				notification();
				break;
		}
	}

	DBusConnection *conn;
	if (!init_bluetooth(&conn))
		return EXIT_FAILURE;

	DeviceList devices = {0};
	scan_bluetooth_devices(conn, &devices, &config);

	if (config.verbose_output)
		print_verbose_output(&devices);
	else
		print_short_output(&devices);

	free_device_list(&devices);
	dbus_connection_unref(conn);

	return EXIT_SUCCESS;
}
