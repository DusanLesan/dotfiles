#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

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

void insert_tag(sqlite3 *db, const char *tag) {
	char *sql = sqlite3_mprintf("INSERT OR IGNORE INTO tags (tag) VALUES (TRIM('%q'));", tag);
	sqlite3_exec(db, sql, 0, 0, 0);
	sqlite3_free(sql);
}

void associate_tag(sqlite3 *db, int image_id, const char *tag) {
	char *sql = sqlite3_mprintf(
		"INSERT OR IGNORE INTO image_tags (image_id, tag_id) "
		"SELECT %d, id FROM tags WHERE tag = TRIM('%q');", image_id, tag);
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
			insert_tag(db, token);
			associate_tag(db, image_id, token);
		}
		token = strtok(NULL, ",");
	}

	remove_old_tags(db, image_id, tags_csv);
}

void print_tags_from_image(const char *image_path) {
	char* tags_csv = get_tags_from_image(image_path);
	if (tags_csv[0] == '\0') return;

	char command[1024];
	snprintf(command, sizeof(command), "notify-send 'Keywords:\n' '%s'", tags_csv);
	system(command);
}

char* get_tags_from_db(sqlite3 *db, const char *image_path) {
	int image_id = get_image_id(db, image_path);

	if (image_id > 0) {
		char db_tags[1024] = {0};

		char *sql = sqlite3_mprintf(
			"SELECT GROUP_CONCAT(tag, ',') FROM tags INNER JOIN image_tags ON tags.id = image_tags.tag_id "
			"WHERE image_tags.image_id = %d;", image_id);

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

void sync_tags_to_image(sqlite3 *db, const char *image_path) {
	char* db_tags = get_tags_from_db(db, image_path);
	if (db_tags[0] == '\0') return;

	char command[1024];
	snprintf(command, sizeof(command), "exiftool -overwrite_original -Keywords=\"%s\" \"%s\"", db_tags, image_path);
	system(command);

}

void print_tags_from_db(sqlite3 *db, const char *image_path) {
	char* db_tags = get_tags_from_db(db, image_path);
	if (db_tags[0] == '\0') return;

	char command[1024];
	snprintf(command, sizeof(command), "notify-send 'Keywords:\n' '%s'", db_tags);
	system(command);
}

void add_keywords(sqlite3 *db, const char *image_path, const char *keywords) {
	int image_id = get_image_id(db, image_path);
	if (!image_id) {
		insert_image(db, image_path);
		image_id = get_image_id(db, image_path);
	}

	char *token = strtok((char *)keywords, ",");
	while (token != NULL) {
		insert_tag(db, token);
		associate_tag(db, image_id, token);
		token = strtok(NULL, ",");
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

void search_for_paths(sqlite3 *db, const char *query, const char *lf_id) {
	char *sql = sqlite3_mprintf(
		"SELECT i.path FROM images AS i "
		"JOIN image_tags AS it ON i.id = it.image_id "
		"JOIN tags AS t ON t.id = it.tag_id "
		"WHERE t.tag = TRIM('%q');", query);

	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
		char paths[1024] = {0};
		char* path;

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			if (lf_id != NULL) {
				path = (char *)sqlite3_column_text(stmt, 0);
				snprintf(paths + strlen(paths), sizeof(paths) - strlen(paths), " \"%s\"", path);
			} else {
				printf("%s\n", sqlite3_column_text(stmt, 0));
			}
		}

		if (paths[0] != '\0') {
			toggle_files(lf_id, paths);
		}
	}

	sqlite3_finalize(stmt);
	sqlite3_free(sql);
}

void read_user_input(char *input, size_t size) {
	if (fgets(input, size, stdin) != NULL) {
		size_t len = strlen(input);
		if (len > 0 && input[len - 1] == '\n') {
			input[len - 1] = '\0';
		}
	} else {
		printf("Error reading input.\n");
	}
}

int main(int argc, char **argv) {
	char *db_path, *image_path, *add_tags, *remove_tags, *lf_id = NULL;
	int sync_from_image, sync_to_image, print_tags, update_tags, opt, search = 0;

	while ((opt = getopt(argc, argv, "D:i:a:r:l:spufd")) != -1) {
		switch (opt) {
			case 'D': db_path = optarg; break;
			case 'i': image_path = optarg; break;
			case 'a': add_tags = optarg; break;
			case 'r': remove_tags = optarg; break;
			case 's': search = 1; break;
			case 'l': lf_id = optarg; break;
			case 'p': print_tags = 1; break;
			case 'u': update_tags = 1; break;
			case 'f': sync_from_image = 1; break;
			case 'd': sync_to_image = 1; break;
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
		char search_query[1024];
		read_user_input(search_query, sizeof(search_query));
		search_for_paths(db, search_query, lf_id);
	} else if (image_path) {
		if (update_tags) {
			if (sync_from_image) {
				sync_tags_from_image(db, image_path);
			} else if (sync_to_image) {
				sync_tags_to_image(db, image_path);
			}
		}

		if (print_tags) {
			if (sync_from_image) {
				print_tags_from_image(image_path);
			} else if (sync_to_image) {
				print_tags_from_db(db, image_path);
			}
		}

		if (add_tags)
			add_keywords(db, image_path, add_tags);
		if (remove_tags)
			remove_keywords(db, image_path, remove_tags);
	}

	sqlite3_close(db);
	return 0;
}
