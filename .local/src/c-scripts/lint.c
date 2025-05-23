#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

char current_file[1024];
int current_line = 0;
int current_line_reported = 0;
int previous_was_empty = 0;
char previous_indent = '\t';

const char *blacklisted_extensions[] = {".png", ".jpeg", ".jpg", ".webp", ".gif", ".mp4", ".jar", ".gz", ".zip", NULL};
char **blacklisted_directories = NULL;

int ignore_consecutive_spaces = 0;
int ignore_trailing_whitespace = 0;
int ignore_empty_lines = 0;

struct IgnoreOption {
	const char *name;
	int *flag;
};

void parse_blacklist_dirs(char *dir_list) {
	static char *default_blacklisted_directories[] = { "node_modules", "build", "out", "vendor", ".git", NULL};

	if (!dir_list) {
		blacklisted_directories = default_blacklisted_directories;
		return;
	}

	size_t initial_count = 5;
	size_t blacklisted_count = 1;
	char *token = strtok(dir_list, ",");
	for (char *p = dir_list; *p; p++) {
		if (*p == ',') blacklisted_count++;
	}

	char **merged_list = malloc((blacklisted_count + initial_count + 1) * sizeof(char *));
	if (!merged_list) {
		perror("Memory allocation failed");
		exit(1);
	}

	for (size_t i = 0; i < initial_count; i++) {
		merged_list[i] = default_blacklisted_directories[i];
	}

	token = strtok(dir_list, ",");
	size_t i = initial_count;
	while (token) {
		merged_list[i++] = token;
		token = strtok(NULL, ",");
	}

	merged_list[i] = NULL;
	blacklisted_directories = merged_list;
}

void parse_ignore_options(char *ignore_list) {
	static struct IgnoreOption options[] = {
		{"consecutive_spaces", &ignore_consecutive_spaces},
		{"trailing_whitespace", &ignore_trailing_whitespace},
		{"empty_lines", &ignore_empty_lines},
		{NULL, NULL}
	};

	char *token = strtok(ignore_list, ",");
	while (token) {
		for (struct IgnoreOption *opt = options; opt->name; opt++) {
			if (strcmp(token, opt->name) == 0) {
				*(opt->flag) = 1;
				break;
			}
		}
		token = strtok(NULL, ",");
	}
}

int is_blacklisted(const char *file_path) {
	for (int i = 0; blacklisted_directories[i] != NULL; i++) {
		if (strstr(file_path, blacklisted_directories[i])) {
			return 1;
		}
	}

	const char *ext = strrchr(file_path, '.');
	if (!ext) return 0;

	for (int i = 0; blacklisted_extensions[i] != NULL; i++) {
		if (strcasecmp(ext, blacklisted_extensions[i]) == 0) {
			return 1;
		}
	}
	return 0;
}

void print_issue(const char *issue, const char *line_content) {
	if (!current_line_reported) {
		current_line_reported = 1;
		printf("\n---------------------------------\n%s:%d\n%s", current_file, current_line, line_content);
		printf("%s", issue);
	} else {
		printf(", %s", issue);
	}
}

char* trim_whitespace(char *input) {
	int found_mixed_indent = 0, line_start = 0;

	for (int i = 0; input[i] != '\0'; i++) {
		if (input[i] == ' ' || input[i] == '\t') {
			if (previous_indent && previous_indent != input[i]) {
				found_mixed_indent = 1;
			}
			previous_indent = input[i];
		} else {
			line_start = i;
			if (input[line_start] == '\n') print_issue("Empty line contains whitespaces", input);
			break;
		}
	}

	if (found_mixed_indent) {
		print_issue("Line contains mixed indentation", input);
		previous_indent = 0;
	}

	return input + line_start;
}

int find_first_char_position(const char *str, char target) {
	int pos = 0;
	while (str[pos] && str[pos] != target) {
		pos++;
	}
	return pos;
}

int find_last_char_position(const char *str, char target) {
	int pos = strlen(str) - 1;
	while (pos >= 0 && str[pos] != target) {
		pos--;
	}
	return pos;
}

int find_pattern_in_substring(const char *input, int start, int end, const char *delimiter, const char *pattern) {
	char *substring = strndup(input + start, end - start);
	if (!substring) {
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}

	char *token = strtok(substring, delimiter);
	int found = 0;
	while (token) {
		if (!strstr(token, pattern)) {
			found = 1;
			break;
		}
		token = strtok(NULL, delimiter);
	}
	free(substring);
	return found;
}

void matches_end_statement(char *line, const char *end_statements[]) {
	for (size_t i = 0; end_statements[i] != NULL; i++) {
		if (strcmp(line, end_statements[i]) == 0) {
			print_issue("Empty line before 'end' statement", line);
			return;
		}
	}
}

void check_xml_issues(char *line) {
	for (char *c = line; *c; c++) {
		if (*c == '>') {
			if (*(c - 1) == '/' && *(c - 2) != ' ') {
				print_issue("Closing tag should have a space before '/'", line);
			}
		} else if (*c == '=') {
			if (*(c + 1) == ' ' || *(c - 1) == ' ') {
				print_issue("Equals should not have spaces around it", line);
			}
		}
	}
}

void check_brs_issues(char *line) {
	line = trim_whitespace(line);

	if (strncmp(line, "if ", 3) == 0 && !strstr(line, " then")) {
		print_issue("Missing 'then' in 'if' statement", line);
	} else if (strncmp(line, "function ", 9) == 0 || strncmp(line, "sub ", 4) == 0) {
		if (line[strlen(line) - 3] != '(') {
			int start = find_first_char_position(line, '(') + 1;
			int end = find_last_char_position(line, ')');
			if (find_pattern_in_substring(line, start, end, ",", " as ")) {
				print_issue("Function has no input type", line);
			}
		} else if (line[0] == 'f') {
			print_issue("Function has no return type", line);
		}
	}

	if (previous_was_empty) {
		const char *end_statements[] = {"end if", "end sub", "end while", "end for", "end function", NULL};
		matches_end_statement(line, end_statements);
	}
}

void validate_line(char *line_content){
	current_line_reported = 0;

	if (line_content[0] == '\n') {
		if (!ignore_empty_lines) {
			if (previous_was_empty) {
				print_issue("Multiple empty lines detected\n", line_content);
			}
			previous_was_empty = 1;
		}
		return;
	}

	line_content = trim_whitespace(line_content);
	size_t len = strlen(line_content);
	if (len < 2) return;

	if (!ignore_trailing_whitespace && line_content[len - 2] == ' ' || line_content[len - 2] == '\t') {
		print_issue("Line ends with whitespace", line_content);
	}

	if (!ignore_consecutive_spaces && strstr(line_content, "  ")) {
		print_issue("Line contains two or more consecutive spaces", line_content);
	}

	if (strstr(current_file, ".brs")) {
		check_brs_issues(line_content);
	} else if (strstr(current_file, ".xml")) {
		check_xml_issues(line_content);
	}

	if (current_line_reported) printf("\n");
	previous_was_empty = 0;
}

void validate_file(const char *file_path) {
	FILE *file = fopen(file_path, "r");
	if (!file) {
		perror("Failed to open file");
		exit(1);
	}

	snprintf(current_file, sizeof(current_file), "%s", file_path);

	previous_indent = 0;
	current_line = 0;
	current_line_reported = 0;
	char line[1024];
	while (fgets(line, sizeof(line), file)) {
		current_line++;
		validate_line(line);
	}
	fclose(file);
}

void validate_directory(const char *path) {
	DIR *dir = opendir(path);
	if (!dir) {
		perror("Failed to open directory");
		exit(1);
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		char full_path[1024];
		snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

		if (entry->d_type == DT_DIR) {
			validate_directory(full_path);
		} else if (entry->d_type == DT_REG) {
			if (is_blacklisted(full_path)) continue;
			validate_file(full_path);
		}
	}

	closedir(dir);
}

void process_file_list_from_stdin() {
	char line[1024];
	while (fgets(line, sizeof(line), stdin)) {
		line[strcspn(line, "\n")] = 0;
		if (is_blacklisted(line)) continue;
		validate_file(line);
	}
}

void process_files_from_path(const char *path) {
	struct stat statbuf;
	if (stat(path, &statbuf) == 0) {
		if (S_ISDIR(statbuf.st_mode)) {
			validate_directory(path);
		} else if (S_ISREG(statbuf.st_mode)) {
			validate_file(path);
		} else {
			fprintf(stderr, "The path is neither a regular file nor a directory: %s\n", path);
		}
	} else {
		perror("stat failed");
	}
}

int main(int argc, char *argv[]) {
	int opt;
	char *ignore_list = NULL, *blacklist_dirs = NULL;
	const char *path = NULL;
	int use_stdin = 0;

	while ((opt = getopt(argc, argv, "p:i:d:s")) != -1) {
		switch (opt) {
			case 'p': path = optarg; break;
			case 'i': ignore_list = optarg; break;
			case 'd': blacklist_dirs = optarg; break;
			case 's': use_stdin = 1; break;
			default: fprintf(stderr, "Invalid option.\n"); return 1;
		}
	}

	if (ignore_list) {
		parse_ignore_options(ignore_list);
	}

	parse_blacklist_dirs(blacklist_dirs);

	if (use_stdin) {
		process_file_list_from_stdin();
	} else if (path) {
		process_files_from_path(path);
	} else {
		fprintf(stderr, "No path or stdin provided.\n");
		return 1;
	}

	return 0;
}
