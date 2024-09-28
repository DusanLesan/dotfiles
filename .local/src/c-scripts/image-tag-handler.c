#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <libnotify/notify.h>

void notify_send(const char *header, const char *message) {
	printf("%s %s\n", header, message);
	notify_init("Notification");
	NotifyNotification *n = notify_notification_new(header, message, NULL);
	notify_notification_show(n, NULL);
	g_object_unref(G_OBJECT(n));
	notify_uninit();
}

char* get_db_path() {
	static char filepath[PATH_MAX];
	char cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("getcwd() error");
		return NULL;
	}

	struct stat buffer;
	while (1) {
		snprintf(filepath, sizeof(filepath), "%s/%s", cwd, ".image.db");

		if (stat(filepath, &buffer) == 0)
			return filepath;

		if (strcmp(cwd, "/") == 0)
			break;

		char *last_slash = strrchr(cwd, '/');
		if (last_slash != NULL)
			*last_slash = '\0';
	}

	return NULL;
}

sqlite3* open_db(const char *db_path) {
	sqlite3 *db;
	if (sqlite3_open(db_path, &db)) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	return db;
}

int get_image_id(sqlite3 *db, const char *image_path) {
	char *sql = sqlite3_mprintf("SELECT id FROM images WHERE path='%q';", image_path);
	sqlite3_stmt *stmt;
	int image_id = 0;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			image_id = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_free(sql);

	return image_id;
}

void insert_image(sqlite3 *db, const char *image_path) {
	char *sql = sqlite3_mprintf("INSERT INTO images (path) VALUES ('%q');", image_path);
	sqlite3_exec(db, sql, 0, 0, 0);
	sqlite3_free(sql);
}

void insert_tag(sqlite3 *db, const char *tags_table, const char *tag_value) {
	char *sql = sqlite3_mprintf("INSERT OR IGNORE INTO %q (tag) VALUES (TRIM('%q'));", tags_table, tag_value);
	sqlite3_exec(db, sql, 0, 0, 0);
	sqlite3_free(sql);
}

void associate_tag(sqlite3 *db, int image_id, const char *tags_table, const char *tag_value) {
	char *sql = sqlite3_mprintf(
		"INSERT OR IGNORE INTO image_%q (image_id, tag_id) "
		"SELECT %d, id FROM %q WHERE tag = TRIM('%q');", tags_table, image_id, tags_table, tag_value);

	sqlite3_exec(db, sql, 0, 0, 0);
	sqlite3_free(sql);
}

void disassociate_tag(sqlite3 *db, int image_id, const char *tag) {
	char *sql = sqlite3_mprintf(
		"DELETE FROM image_tags "
		"WHERE image_id = %d AND tag_id = (SELECT id FROM tags WHERE tag = '%q');", image_id, tag);
	sqlite3_exec(db, sql, 0, 0, 0);
	sqlite3_free(sql);
}

void remove_old_tags(sqlite3 *db, int image_id, const char *tags_csv) {
	char *sql = sqlite3_mprintf(
		"SELECT tag FROM tags INNER JOIN image_tags ON tags.id = image_tags.tag_id "
		"WHERE image_tags.image_id = %d;", image_id);

	sqlite3_stmt *stmt;
	char existing_tags[1024] = {0};

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			const char *existing_tag = (const char *)sqlite3_column_text(stmt, 0);
			if (existing_tags[0] != '\0') {
				strncat(existing_tags, ",", sizeof(existing_tags) - strlen(existing_tags) - 1);
			}
			strncat(existing_tags, existing_tag, sizeof(existing_tags) - strlen(existing_tags) - 1);
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_free(sql);

	char *token = strtok(existing_tags, ",");
	while (token != NULL) {
		if (!strstr(tags_csv, token)) {
			disassociate_tag(db, image_id, token);
		}
		token = strtok(NULL, ",");
	}
}

char* get_tags_from_image(const char *image_path) {
	char command[1024];
	snprintf(command, sizeof(command), "exiftool -s -s -s -Keywords '%s'", image_path);
	FILE *fp = popen(command, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to run command\n");
		return NULL;
	}

	char tags_csv[1024];
	if (fgets(tags_csv, sizeof(tags_csv), fp) == NULL) {
		pclose(fp);
		fprintf(stderr, "No tags found or failed to read output\n");
		return NULL;
	}
	pclose(fp);

	tags_csv[strcspn(tags_csv, "\n")] = '\0'; // Replace newline with null
	
	return strdup(tags_csv);
}

void sync_tags_from_image(sqlite3 *db, const char *image_path) {
	char* tags_csv = get_tags_from_image(image_path);
	if (tags_csv[0] == '\0') return;

	int image_id = get_image_id(db, image_path);
	if (!image_id) {
		insert_image(db, image_path);
		image_id = get_image_id(db, image_path);
	}

	char *token = strtok(tags_csv, ",");
	while (token) {
		if (*token) {
			insert_tag(db, "keywords", token);
			associate_tag(db, image_id, "keywords", token);
		}
		token = strtok(NULL, ",");
	}

	remove_old_tags(db, image_id, tags_csv);
}

void print_tags_from_image(const char *image_path) {
	char* tags_csv = get_tags_from_image(image_path);
	if (tags_csv == NULL) return;
	notify_send("Keywords", tags_csv);
}

char* get_tags_from_db(sqlite3 *db, const char *image_path, const char *tag_name) {
	int image_id = get_image_id(db, image_path);

	if (image_id > 0) {
		char db_tags[1024] = {0};

		char *sql = sqlite3_mprintf(
			"SELECT GROUP_CONCAT(tag, ',') FROM %q AS t INNER JOIN image_%q AS it ON t.id = it.tag_id "
			"WHERE it.image_id = %d;", tag_name, tag_name, image_id);

		sqlite3_stmt *stmt;
		if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
			if (sqlite3_step(stmt) == SQLITE_ROW) {
				const char *tags = (const char *)sqlite3_column_text(stmt, 0);
				if (tags) {
					strncpy(db_tags, tags, sizeof(db_tags) - 1);
				}
			}
		}
		sqlite3_finalize(stmt);
		sqlite3_free(sql);

		return strdup(db_tags);
	} else {
		fprintf(stderr, "Image not found in database.\n");
		return NULL;
	}
}

void sync_tags_to_image(sqlite3 *db, const char *image_path, const char *tag_name) {
	char* db_tags = get_tags_from_db(db, image_path, tag_name);
	if (db_tags[0] == '\0') return;

	char command[1024];
	snprintf(command, sizeof(command), "exiftool -overwrite_original -Keywords=\"%s\" \"%s\"", db_tags, image_path);
	system(command);

}

void print_tags_from_db(sqlite3 *db, const char *image_path, const char *tag_name) {
	char* db_tags = get_tags_from_db(db, image_path, tag_name);
	if (db_tags == NULL) return;
	notify_send(tag_name, db_tags);
}

void add_tags(sqlite3 *db, const char *image_path, const char *tag_name, const char *tag_value_csv) {
	int image_id = get_image_id(db, image_path);
	if (!image_id) {
		insert_image(db, image_path);
		image_id = get_image_id(db, image_path);
	}

	char *tag_value, *saveptr;
	tag_value = strtok_r((char *)tag_value_csv, ",", &saveptr);
	while (tag_value != NULL) {
		insert_tag(db, tag_name, tag_value);
		associate_tag(db, image_id, tag_name, tag_value);
		tag_value = strtok_r(NULL, ",", &saveptr);
	}
}

void remove_keywords(sqlite3 *db, const char *image_path, const char *keywords) {
	int image_id = get_image_id(db, image_path);
	if (image_id) {
		char *token = strtok((char *)keywords, ",");
		while (token != NULL) {
			disassociate_tag(db, image_id, token);
			token = strtok(NULL, ",");
		}
	}
}

void toggle_files(const char *lf_id, const char *image_path) {
	if (!lf_id) return;
	char command[1024];
	snprintf(command, sizeof(command), "lf -remote 'send %s toggle %s'", lf_id, image_path);
	system(command);
}

void search_for_paths(sqlite3 *db, const char *tag_name, const char *query, const char *lf_id) {
	char *sql = sqlite3_mprintf(
		"SELECT i.path FROM images AS i "
		"JOIN image_%q AS it ON i.id = it.image_id "
		"JOIN %q AS t ON t.id = it.tag_id "
		"WHERE t.tag = TRIM('%q');", tag_name, tag_name, query);

	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
		char paths[990] = {0};
		char *path;

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			if (lf_id != NULL) {
				path = (char *)sqlite3_column_text(stmt, 0);
				if (strlen(paths) + strlen(path) + 3 >= sizeof(paths)) {
					toggle_files(lf_id, paths);
					memset(paths, 0, sizeof(paths));
				}

				snprintf(paths + strlen(paths), sizeof(paths) - strlen(paths), " \"%s\"", path);
			} else {
				printf("%s\n", sqlite3_column_text(stmt, 0));
			}
		}

		if (strlen(paths) > 0) {
			toggle_files(lf_id, paths);
		}
	}

	sqlite3_finalize(stmt);
	sqlite3_free(sql);
}

char* read_user_input() {
	char input[32];
	fflush(stdout);

	if (fgets(input, sizeof(input), stdin) != NULL) {
		size_t len = strlen(input);
		if (len > 0 && input[len - 1] == '\n') {
			input[len - 1] = '\0';
		}
	} else {
		printf("Error reading input.\n");
	}
	return strdup(input);
}

int main(int argc, char **argv) {
	char *db_path = NULL, *image_paths = NULL, *lf_id = NULL , *tag_name = "keywords", *tag_value = NULL;
	int opt, target_file = 0, target_db = 0, print = 0, update = 0, should_add_tags = 0, should_remove_tags = 0, search = 0;

	while ((opt = getopt(argc, argv, "D:i:t:l:spufdar")) != -1) {
		switch (opt) {
			case 'D': db_path = optarg; break;
			case 'i': image_paths = optarg; break;
			case 't': tag_name = optarg; break;
			case 'l': lf_id = optarg; break;
			case 's': search = 1; break;
			case 'p': print = 1; break;
			case 'u': update = 1; break;
			case 'f': target_file = 1; break;
			case 'd': target_db = 1; break;
			case 'a': should_add_tags = 1; break;
			case 'r': should_remove_tags = 1; break;
			default: fprintf(stderr, "Invalid option.\n"); return 1;
		}
	}

	if (!db_path) {
		db_path = get_db_path();
		if (!db_path) {
			fprintf(stderr, "Database path is not found.\n");
			return 1;
		}
	}

	sqlite3 *db = open_db(db_path);
	if (!db)
		return 1;

	if (search) {
		search_for_paths(db, tag_name, read_user_input(), lf_id);
	} else if (image_paths) {
		if (should_add_tags || should_remove_tags)
			tag_value = read_user_input();

		char *image_path, *saveptr;
		image_path = strtok_r(image_paths, "\n", &saveptr);
		while (image_path != NULL) {
			if (update) {
				if (target_db) {
					sync_tags_from_image(db, image_path);
				} else if (target_file) {
					sync_tags_to_image(db, image_path, tag_name);
				}
			}

			if (print) {
				if (target_file) {
					print_tags_from_image(image_path);
				} else if (target_db) {
					print_tags_from_db(db, image_path, tag_name);
				}
			}

			if (should_add_tags) {
				add_tags(db, image_path, tag_name, tag_value);
			} else if (should_remove_tags) {
				remove_keywords(db, image_path, tag_value);
			}

			image_path = strtok_r(NULL, "\n", &saveptr);
		}
	}

	sqlite3_close(db);
	return 0;
}
