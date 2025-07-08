#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <sqlite3.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <libnotify/notify.h>
#include <openssl/evp.h>

static char *db_path = NULL;
static char *lf_id = NULL;

void notify_send(const char *header, const char *message, int is_error) {
	if (lf_id) {
		notify_init("Notification");
		NotifyNotification *n = notify_notification_new(header, message, NULL);
		notify_notification_show(n, NULL);
		g_object_unref(G_OBJECT(n));
		notify_uninit();
	} else if (is_error) {
		fprintf(stderr, "%s: %s\n", header, message);
	} else {
		if (header)
			fprintf(stdout, "%s: %s\n", header, message);
		else
			fprintf(stdout, "%s\n", message);
	}
}

void die(const char *error_header, const char *error) {
	if (!error_header)
		error_header = "Error";

	notify_send(error_header ? error_header : "Error", error, 1);
	exit(EXIT_FAILURE);
}

char* dmenu_prompt(const char* dmenu_prompt, const char* options_newline) {
	char cmd[strlen(dmenu_prompt) + strlen(options_newline) + 32];
	FILE *fp;
	char *output = NULL;
	size_t output_len = 0;

	if (options_newline)
		snprintf(cmd, sizeof(cmd), "dmenu -p '%s' <<< '%s'", dmenu_prompt, options_newline);
	else
		snprintf(cmd, sizeof(cmd), "dmenu -p \"%s\" <<< ''", dmenu_prompt);

	fp = popen(cmd, "r");
	if (fp == NULL)
		die("Error", "Failed to run command");

	ssize_t nread = getline(&output, &output_len, fp);
	if (nread == -1)
		die(NULL, "Failed to read output from dmenu");

	output[nread - 1] = '\0'; // Replace newline with null
	pclose(fp);

	return output;
}

char* read_user_input(char *prompt) {
	char input[32];
	fflush(stdout);
	printf("%s", prompt);

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

void create_empty_db(const char *path) {
	char *schema =
		"CREATE TABLE images (id INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT UNIQUE, sha256sum TEXT);"
		"CREATE TABLE keywords (id INTEGER PRIMARY KEY AUTOINCREMENT, tag TEXT UNIQUE);"
		"CREATE TABLE image_keywords (image_id INTEGER, tag_id INTEGER, "
		"FOREIGN KEY (image_id) REFERENCES images(id) ON DELETE CASCADE, "
		"FOREIGN KEY (tag_id) REFERENCES keywords(id) ON DELETE RESTRICT, UNIQUE(image_id, tag_id));"
		"CREATE INDEX idx_image_keywords_image_id ON image_keywords(image_id);"
		"CREATE INDEX idx_image_keywords_tag_id ON image_keywords(tag_id);"
		"CREATE TABLE subjects (id INTEGER PRIMARY KEY AUTOINCREMENT, tag TEXT UNIQUE);"
		"CREATE TABLE image_subjects (image_id INTEGER, tag_id INTEGER, "
		"FOREIGN KEY(image_id) REFERENCES images(id) ON DELETE CASCADE, "
		"FOREIGN KEY(tag_id) REFERENCES subjects(id) ON DELETE RESTRICT, UNIQUE(image_id, tag_id));"
		"CREATE INDEX idx_image_subjects_image_id ON image_subjects(image_id);"
		"CREATE INDEX idx_image_subjects_tag_id ON image_subjects(tag_id);"
		"CREATE TABLE shared (id INTEGER PRIMARY KEY AUTOINCREMENT, tag TEXT UNIQUE);"
		"CREATE TABLE image_shared (image_id INTEGER, tag_id INTEGER, "
		"FOREIGN KEY (image_id) REFERENCES images(id) ON DELETE CASCADE, "
		"FOREIGN KEY (tag_id) REFERENCES shared(id) ON DELETE RESTRICT, UNIQUE(image_id, tag_id));"
		"CREATE INDEX idx_image_shared_image_id ON image_shared(image_id);"
		"CREATE INDEX idx_image_shared_tag_id ON image_shared(tag_id);";
	sqlite3 *db;
	if (sqlite3_open(path, &db) == SQLITE_OK)
		sqlite3_exec(db, schema, 0, 0, 0);
	sqlite3_close(db);
}

char* get_user_input(char *lf_id, char *prompt, char *options_newline) {
	if (lf_id)
		return dmenu_prompt(prompt, options_newline);
	else
		return read_user_input(prompt);
}

void trimArray(char* array) {
	while (array[strlen(array) - 1] == '\n' || array[strlen(array) - 1] == ',') {
		array[strlen(array) - 1] = '\0';
	}
}

size_t reallocateArray(char** array, size_t current_allocation, size_t required_length) {
	if (required_length > current_allocation) {
		while (required_length > current_allocation) {
			current_allocation *= 2;
		}

		char* new_output = realloc(*array, current_allocation);
		if (!new_output)
			die(NULL, "Failed to reallocate memory");
		*array = new_output;
	}

	return current_allocation;
}

char* format_file_paths(const char* file_paths, const char* format) {
	size_t current_allocation = 32;
	char* query = malloc(current_allocation);
	if (!query)
		die(NULL, "Failed to allocate memory");

	char *input_copy = strdup(file_paths);
	if (!input_copy)
		die(NULL, "Failed to allocate memory for input copy");

	query[0] = '\0';
	size_t current_length = 0;
	size_t padding_length = strlen(format) - 1;

	char *token = strtok(input_copy, "\n");
	while (token != NULL) {
		size_t new_path_length = strlen(token) + padding_length;
		size_t required_length = current_length + new_path_length + 1;
		current_allocation = reallocateArray(&query, current_allocation, required_length);

		snprintf(query + current_length, new_path_length + 1, format, token);
		current_length = strlen(query);
		token = strtok(NULL, "\n");
	}

	trimArray(query);

	return query;
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

		if (cwd[0] == '\0' || strcmp(cwd, "/") == 0)
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
		die("Can't open database:", sqlite3_errmsg(db));
		return NULL;
	}
	sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
	return db;
}

void insert_images(sqlite3 *db, char *image_paths){
	char *formatted_image_paths = format_file_paths(image_paths, "(TRIM('%s')),\n");
	char *sql = sqlite3_mprintf(
		"INSERT OR IGNORE INTO images (path) VALUES\n%s;", formatted_image_paths);

	sqlite3_exec(db, sql, 0, 0, 0);
}

char* get_statement_result(sqlite3 *db, const char *sql) {
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			return (char *) sqlite3_column_text(stmt, 0);
		}
	}

	return NULL;
}

char* get_tag_id(sqlite3 *db, const char *tag_name, const char *tag_value) {
	char *sql = sqlite3_mprintf(
		"SELECT id FROM %q WHERE tag = TRIM('%q');", tag_name, tag_value);

	return get_statement_result(db, sql);
}

char* get_options(sqlite3 *db, const char *tag_name) {
	char *sql = sqlite3_mprintf(
		"SELECT GROUP_CONCAT(t.tag, '\n') from %s t ", tag_name);
	char *res = get_statement_result(db, sql);
	sqlite3_free(sql);

	return res ? res : "";
}

char* get_image_ids(sqlite3 *db, const char *image_paths) {
	char *formatted_image_paths = format_file_paths(image_paths, "TRIM('%s'),\n");
	char *sql = sqlite3_mprintf(
		"SELECT id FROM images WHERE path IN (%s);", formatted_image_paths);

	char *image_id;
	char* image_ids = malloc(256);
	image_ids[0] = '\0';
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			image_id = (char *)sqlite3_column_text(stmt, 0);
			snprintf(image_ids + strlen(image_ids), sizeof(image_id) + 1, "%s\n", image_id);
		}
	} else {
		die("Can't get image ids:", sqlite3_errmsg(db));
	}

	return image_ids;
}

void insert_tag(sqlite3 *db, const char *tags_table, const char *tag_value) {
	char *sql = sqlite3_mprintf("INSERT OR IGNORE INTO %q (tag) VALUES (TRIM('%q'));", tags_table, tag_value);
	sqlite3_exec(db, sql, 0, 0, 0);
	sqlite3_free(sql);
}

void associate_tag(sqlite3 *db, const char *image_id, const char *tags_table, const char *tag_value) {
	char *sql = sqlite3_mprintf(
		"INSERT OR IGNORE INTO image_%q (image_id, tag_id) "
		"SELECT %d, id FROM %q WHERE tag = TRIM('%q');", tags_table, image_id, tags_table, tag_value);

	sqlite3_exec(db, sql, 0, 0, 0);
	sqlite3_free(sql);
}

void disassociate_tag(sqlite3 *db, const char *image_ids, const char *tag_name, const char *tag_id) {
	char *formatted_sql_template = format_file_paths(image_ids, "%s,\n");
	char *sql = sqlite3_mprintf(
		"DELETE FROM image_%q WHERE tag_id = %s AND image_id IN (%s);", tag_name, tag_id, formatted_sql_template);

	sqlite3_exec(db, sql, 0, 0, 0);
	sqlite3_free(sql);
}

void remove_old_tags(sqlite3 *db, const char *image_id, const char *tag_name, const char *tags_csv) {
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
			disassociate_tag(db, image_id, tag_name, token);
		}
		token = strtok(NULL, ",");
	}
}

char* get_tags_from_image(const char *image_path) {
	char command[1024];
	snprintf(command, sizeof(command), "exiftool -s -s -s -Keywords '%s'", image_path);
	FILE *fp = popen(command, "r");
	if (fp == NULL) {
		notify_send("Error", "Failed to run command", 1);
		return NULL;
	}

	char tags_csv[1024];
	if (fgets(tags_csv, sizeof(tags_csv), fp) == NULL) {
		pclose(fp);
		notify_send("Error", "No tags found or failed to read output", 1);
		return NULL;
	}
	pclose(fp);

	tags_csv[strcspn(tags_csv, "\n")] = '\0'; // Replace newline with null

	return strdup(tags_csv);
}

void sync_tags_from_image(sqlite3 *db, char *image_path) {
	char* tags_csv = get_tags_from_image(image_path);
	if (tags_csv[0] == '\0') return;

	insert_images(db, image_path);
	char *image_id = get_image_ids(db, image_path);

	char *token = strtok(tags_csv, ",");
	while (token) {
		if (*token) {
			insert_tag(db, "keywords", token);
			associate_tag(db, image_id, "keywords", token);
		}
		token = strtok(NULL, ",");
	}

	remove_old_tags(db, image_id, "keywords", tags_csv);
}

void print_tags_from_image(const char *image_path) {
	char* tags_csv = get_tags_from_image(image_path);
	if (tags_csv == NULL) return;
	notify_send("Keywords", tags_csv, 0);
}

static void* free_all(void **ptrs, int count) {
	for (int i = 0; i < count; i++) {
		free(ptrs[i]);
	}
	return NULL;
}

char* get_tags_from_db(sqlite3 *db, const char *image_path, const char *tag_name) {
	char *tags = strdup(tag_name);
	if (!tags) return NULL;

	char *selects = malloc(256);
	char *joins = malloc(512);
	char *result = malloc(1024);
	if (!selects || !joins || !result) return free_all((void*[]){tags, selects, joins, result}, 4);

	selects[0] = joins[0] = result[0] = 0;

	strcat(selects, "SELECT ");
	strcat(joins, " FROM images i ");

	char *token = strtok(tags, ",");
	int first = 1;

	while (token) {
		char *tag = token;

		if (!first) strcat(selects, ", ");
		first = 0;

		char buf[256];
		snprintf(buf, sizeof(buf), "GROUP_CONCAT(DISTINCT %s.tag) AS %s", tag, tag);
		strcat(selects, buf);

		snprintf(buf, sizeof(buf),
			" LEFT JOIN image_%s ON image_%s.image_id = i.id"
			" LEFT JOIN %s ON %s.id = image_%s.tag_id",
			tag, tag, tag, tag, tag);
		strcat(joins, buf);

		token = strtok(NULL, ",");
	}

	free(tags);

	char *sql = malloc(strlen(selects) + strlen(joins) + strlen(image_path) + 100);
	if (!sql) return free_all((void*[]){selects, joins, result}, 3);

	sprintf(sql, "%s%s WHERE i.path LIKE '%%%s%%';", selects, joins, image_path);

	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			int col_count = sqlite3_column_count(stmt);
			first = 1;
			for (int i = 0; i < col_count; ++i) {
				const char *val = (const char *)sqlite3_column_text(stmt, i);
				if (val) {
					const char *col_name = sqlite3_column_name(stmt, i);
					if (!first) strcat(result, "\n");
					strcat(result, col_name);
					strcat(result, ": ");
					strcat(result, val);
					first = 0;
				}
			}
		}
		sqlite3_finalize(stmt);
	}

	free_all((void*[]){selects, joins, sql}, 3);

	return (*result) ? result : (free(result), NULL);
}

void sync_tags_to_image(sqlite3 *db, const char *image_path, const char *tag_name) {
	char* db_tags = get_tags_from_db(db, image_path, tag_name);
	if (db_tags && db_tags[0]) {
		char command[1024];
		snprintf(command, sizeof(command), "exiftool -overwrite_original -Keywords=\"%s\" \"%s\"", db_tags, image_path);
		system(command);
	}
	free(db_tags);
}

void print_tags_from_db(sqlite3 *db, const char *image_path, const char *tag_name) {
	char* db_tags = get_tags_from_db(db, image_path, tag_name);
	if (db_tags == NULL || db_tags[0] == '\0')
		die("Error", "No tags found in database");

	notify_send(NULL, db_tags, 0);
	free(db_tags);
}

void add_tags(sqlite3 *db, char *image_ids, const char *tag_name, char *tag_value_csv) {
	char *tag_value_copy = strdup(tag_value_csv);
	char *tag_value = strtok(tag_value_copy, ",");
	while (tag_value != NULL) {
		insert_tag(db, tag_name, tag_value);

		char *tag_id = get_tag_id(db, tag_name, tag_value);
		if (tag_id != NULL) {
			char format[12];
			snprintf(format, sizeof(format), "(%%s,%s),", tag_id);
			char *formatted_sql_template = format_file_paths(image_ids, format);

			char *sql = sqlite3_mprintf(
				"INSERT OR IGNORE INTO image_%q (image_id, tag_id) VALUES"
				"%q;", tag_name, formatted_sql_template);

			sqlite3_exec(db, sql, 0, 0, 0);
			sqlite3_free(sql);
			free(formatted_sql_template);
		}
		tag_value = strtok(NULL, ",");
	}
	free(tag_value_copy);
}

void remove_tags(sqlite3 *db, const char *image_paths, const char *tag_name, const char *tag_value_csv) {
	char *image_ids = get_image_ids(db, image_paths);
	if (image_ids) {
		char *tag_value_copy = strdup(tag_value_csv);
		if (tag_value_copy) {
			char *tag_value = strtok(tag_value_copy, ",");
			while (tag_value != NULL) {
				char *tag_id = get_tag_id(db, tag_name, tag_value);
				if (tag_id != NULL) {
					disassociate_tag(db, image_ids, tag_name, tag_id);
				}
				tag_value = strtok(NULL, ",");
			}
		}
		free_all((void*[]){image_ids, tag_value_copy}, 2);
	}
}

void lf_files_selection(const char *lf_id, const char *action, const char *image_path) {
	if (!lf_id) return;
	char command[1024];
	snprintf(command, sizeof(command), "lf -remote 'send %s %s %s'", lf_id, action, image_path);
	system(command);
}

void create_directory_if_needed(char *path, const char *name) {
	snprintf(path + strlen(path), PATH_MAX - strlen(path), "/%s", name);

	if (stat(path, NULL) != 0)
		mkdir(path, 0755);
}

void create_symlink_tree(const char *db_path, const char *type, const char *name, char *link_path) {
	const char *last_slash = strrchr(db_path, '/');
	snprintf(link_path, last_slash ? (last_slash - db_path + 1) : 0, "%s", db_path);

	create_directory_if_needed(link_path, ".tag_tree");
	create_directory_if_needed(link_path, type);
	create_directory_if_needed(link_path, name);
}

void create_symlink_in_dir(const char *file_path, const char *link_dir) {
	char link_path[PATH_MAX];
	char *base_name = basename((char *)file_path);

	snprintf(link_path, sizeof(link_path), "%s/%s", link_dir, base_name);

	struct stat st;
	if (lstat(link_path, &st) != 0)
		symlink(file_path, link_path);
}

void simple_search_out(sqlite3_stmt *stmt) {
	char paths[990] = {0};
	char *last_path = NULL;

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		const char *path = (const char *)sqlite3_column_text(stmt, 0);
		if (lf_id != NULL) {
			if (strlen(paths) + strlen(path) + 3 >= sizeof(paths)) {
				lf_files_selection(lf_id, "select-path", paths);
				memset(paths, 0, sizeof(paths));
			}

			size_t len = strlen(paths);
			snprintf(paths + len, sizeof(paths) - len, " \"%s\"", path);
			last_path = paths + len;
		} else {
			printf("%s\n", path);
		}
	}

	if (strlen(paths) > 0) {
		lf_files_selection(lf_id, "select-path", paths);
	}

	if (last_path != NULL) {
		lf_files_selection(lf_id, "select", last_path + 1);
		free(last_path);
	}
}

void tree_search_out(sqlite3_stmt *stmt, const char *tag_name, const char *query) {
	char *path;
	char link_path[PATH_MAX];
	create_symlink_tree(db_path, tag_name, query, link_path);

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		create_symlink_in_dir((char *)sqlite3_column_text(stmt, 0), link_path);
	}
}

void file_search_out(sqlite3_stmt *stmt) {
	char filepath[256];
	const char *home = getenv("HOME");
	if (!home)
		die("Error", "Failed to get home directory");

	snprintf(filepath, sizeof(filepath), "%s/.local/share/lf/search", home);
	int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		die("Error", "Failed to create or clear the file");

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		dprintf(fd, "%s\n", (char *)sqlite3_column_text(stmt, 0));
	}
	close(fd);
}

void search_for_paths(sqlite3 *db, const char *tag_name, const char *query, const char *mode) {
	char *sql = sqlite3_mprintf(
		"SELECT i.path FROM images AS i "
		"JOIN image_%q AS it ON i.id = it.image_id "
		"JOIN %q AS t ON t.id = it.tag_id "
		"WHERE t.tag = TRIM('%q') "
		"ORDER BY 1 COLLATE NOCASE;", tag_name, tag_name, query);

	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
		switch (mode[0]) {
			case 'f': file_search_out(stmt); break;
			case 't': tree_search_out(stmt, tag_name, query); break;
			default: simple_search_out(stmt); break;
		}
	}

	sqlite3_finalize(stmt);
	sqlite3_free(sql);
}

char *calculate_sha256(const char *filename) {
	unsigned char hash[EVP_MAX_MD_SIZE];
	char *output_buffer = malloc(2 * EVP_MAX_MD_SIZE + 1);
	if (!output_buffer) return NULL;

	FILE *file = fopen(filename, "rb");
	if (!file) {
		free(output_buffer);
		return NULL;
	}

	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	if (!mdctx || EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
		EVP_MD_CTX_free(mdctx);
		fclose(file);
		free(output_buffer);
		return NULL;
	}

	unsigned char buffer[32768];
	int bytes_read;
	while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
		EVP_DigestUpdate(mdctx, buffer, bytes_read);
	}
	fclose(file);

	if (EVP_DigestFinal_ex(mdctx, hash, NULL) != 1) {
		EVP_MD_CTX_free(mdctx);
		free(output_buffer);
		return NULL;
	}

	EVP_MD_CTX_free(mdctx);

	for (int i = 0; i < 32; i++) {
		sprintf(output_buffer + (i * 2), "%02x", hash[i]);
	}
	output_buffer[2 * EVP_MAX_MD_SIZE] = '\0';

	return output_buffer;
}

int prepare_insert_statement(sqlite3 *db, sqlite3_stmt **stmt) {
	const char *insert_query =
		"INSERT OR REPLACE INTO images (id, path, sha256sum) "
		"VALUES ((SELECT id FROM images WHERE sha256sum = ?), ?, ?)";
	return sqlite3_prepare_v2(db, insert_query, -1, stmt, NULL);
}

int update_image_table(sqlite3 *db, sqlite3_stmt *stmt, const char *sha256sum_value, const char *path_value) {
	int rc;

	sqlite3_bind_text(stmt, 1, sha256sum_value, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, path_value, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, sha256sum_value, -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
		fprintf(stderr, "Failed to execute insert statement: %s\n", sqlite3_errmsg(db));

	sqlite3_reset(stmt);
	return rc == SQLITE_DONE ? SQLITE_OK : rc;
}

int batch_sync(sqlite3 *db, const char *directory_path) {
	int rc;
	sqlite3_stmt *stmt;
	DIR *dir;
	struct dirent *entry;

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	if (rc != SQLITE_OK)
		die("Failed to begin transaction", sqlite3_errmsg(db));

	rc = prepare_insert_statement(db, &stmt);
	if (rc != SQLITE_OK)
		die("Failed to prepare upsert statement", sqlite3_errmsg(db));

	dir = opendir(directory_path);
	if (!dir)
		die("Failed to open directory", "Directory error");

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_DIR)
			continue;

		char file_path[PATH_MAX];
		snprintf(file_path, PATH_MAX, "%s/%s", directory_path, entry->d_name);

		char *file_sha256 = calculate_sha256(file_path);
		if (!file_sha256) {
			fprintf(stderr, "Failed to calculate SHA-256 for file: %s\n", file_path);
			continue;
		}

		rc = update_image_table(db, stmt, file_sha256, file_path);
		free(file_sha256);

		if (rc != SQLITE_OK) {
			fprintf(stderr, "Failed to insert image for file: %s\n", file_path);
			break;
		}
	}

	closedir(dir);
	sqlite3_finalize(stmt);

	if (rc == SQLITE_OK) {
		rc = sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "Failed to commit transaction: %s\n", sqlite3_errmsg(db));
		}
	} else {
		sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
	}

	return rc;
}

int file_exists(const char *filename) {
	struct stat buffer;
	return (stat(filename, &buffer) == 0);
}

void set_column_where_id(sqlite3 *db, const char *table, const char *column, const char *value, int id) {
	sqlite3_stmt *stmt;
	char *query = sqlite3_mprintf("UPDATE %q SET %q = TRIM(?) WHERE id = ?;", table, column);

	if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, value, -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, id);

		if (sqlite3_step(stmt) != SQLITE_DONE) {
			fprintf(stderr, "Failed to set sha256sum: %s\n", sqlite3_errmsg(db));
		}
		sqlite3_finalize(stmt);
	} else {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_free(query);
}

void update_sha256sum(sqlite3 *db) {
	sqlite3_stmt *stmt;
	const char *query = "SELECT id, path, sha256sum FROM images";

	if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK)
		die("Failed to prepare SELECT statement", sqlite3_errmsg(db));

	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		int id = sqlite3_column_int(stmt, 0);
		const char *path = (const char *)sqlite3_column_text(stmt, 1);
		const char *current_sha256 = (const char *)sqlite3_column_text(stmt, 2);

		if (file_exists(path)) {
			char *sha256sum = calculate_sha256(path);
			if (sha256sum) {
				if (!current_sha256 || strcmp(current_sha256, sha256sum) != 0) {
					set_column_where_id(db, "images", "sha256sum", sha256sum, id);
				}
				free(sha256sum);
			}
		} else {
			if (current_sha256) {
				set_column_where_id(db, "images", "sha256sum", "NULL", id);
			}
		}
	}

	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, NULL);
	sqlite3_finalize(stmt);
}

int main(int argc, char **argv) {
	char *image_paths = NULL, *tag_name = NULL, *tag_values = NULL, *sync_path = NULL, *mode = "d";
	int target_file = 0, target_db = 0, should_add_tags = 0, should_remove_tags = 0;
	int opt, search = 0, sync = 0, print = 0, update = 0, help = 0;

	while ((opt = getopt(argc, argv, "D:i:t:l:m:S:spufdarU")) != -1) {
		switch (opt) {
			case 'D': db_path = optarg; break;
			case 'i': image_paths = optarg; break;
			case 't': tag_name = optarg; break;
			case 'l': lf_id = optarg; break;
			case 's': search = 1; break;
			case 'm': mode = optarg; break;
			case 'p': print = 1; break;
			case 'u': update = 1; break;
			case 'f': target_file = 1; break;
			case 'd': target_db = 1; break;
			case 'a': should_add_tags = 1; break;
			case 'r': should_remove_tags = 1; break;
			case 'S': sync_path = optarg; break;
			case 'U': sync = 1; break;
			case '?': help = 1; break;
			default: fprintf(stderr, "Invalid option.\n"); return 1;
		}
	}

	if (help) {
		printf("Usage: %s [OPTIONS]\n", argv[0]);
		printf("Options:\n");
		printf("  -D <path>  Path to the database\n");
		printf("  -i <paths> Image paths\n");
		printf("  -t <name>  Tag name\n");
		printf("  -l <id>    lf id\n");
		printf("  -s         Search for images\n");
		printf("  -m <mode>  Search mode\n");
		printf("  -p         Print tags\n");
		printf("  -u         Update tags\n");
		printf("  -f         Target file\n");
		printf("  -d         Target database\n");
		printf("  -a         Add tags\n");
		printf("  -r         Remove tags\n");
		printf("  -S <path>  Sync path\n");
		printf("  -U         Sync sha256sum\n");
		return 0;
	}

	if (!db_path) {
		db_path = get_db_path();
		if (!db_path && !print) {
			char *user_yes_no = get_user_input(lf_id, "No database found. Create a new one? (y/n): ", "y\nn");
			if (strcmp(user_yes_no, "y") == 0) {
				create_empty_db(".image.db");
				db_path = ".image.db";
			} else {
				die(NULL, "No database found and user declined to create one.");
			}
		}
	}

	sqlite3 *db = open_db(db_path);
	if (!db)
		die(NULL, "Failed to open database");

	if (sync_path) {
		batch_sync(db, sync_path);
	} else if (sync) {
		update_sha256sum(db);
	} else if (search) {
		tag_name = tag_name ? tag_name : get_user_input(lf_id, "Tag name: ", "keywords\nsubjects");
		search_for_paths(db, tag_name, get_user_input(lf_id, "Query: ", get_options(db, tag_name)), mode);
	} else if (image_paths) {
		if (should_add_tags || should_remove_tags) {
			tag_name = tag_name ? tag_name : get_user_input(lf_id, "Tag name: ", "keywords\nsubjects\nshared");
			tag_values = get_user_input(lf_id, "Tag values: ", get_options(db, tag_name));

			if (should_add_tags) {
				insert_images(db, image_paths);
				char *image_ids = get_image_ids(db, image_paths);
				add_tags(db, image_ids, tag_name, tag_values);
			} else if (should_remove_tags) {
				char *image_ids = get_image_ids(db, image_paths);
				remove_tags(db, image_paths, tag_name, tag_values);
			}
		} else {
			char *image_path, *saveptr;
			image_path = strtok_r(image_paths, "\n", &saveptr);
			tag_name = tag_name ? tag_name : "keywords,subjects,shared";
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

				image_path = strtok_r(NULL, "\n", &saveptr);
			}
		}
	}

	sqlite3_close(db);
	return 0;
}
