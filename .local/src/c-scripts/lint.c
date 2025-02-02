#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>

char current_file[1024];
int current_line = 0;
int current_line_reported = 0;
int previous_was_empty = 0;
int ignore_consecutive_spaces = 0;

void print_issue(const char *issue, const char *line_content) {
	if (!current_line_reported) {
		printf("---------------------------------\n%s:%d\n%s\n", current_file, current_line, line_content);
		current_line_reported = 1;
		printf("%s", issue);
	} else {
		printf(", %s", issue);
	}
}

char* trim_whitespace(char *input) {
	while (*input && (isspace((unsigned char)*input))) {
		input++;
	}

	if (*input == '\0') {
		return strdup("");
	}

	const char *end = input + strlen(input) - 1;
	while (end > input && isspace((unsigned char)*end)) {
		end--;
	}

	size_t len = end - input + 1;

	char *trimmed = malloc(len + 1);
	if (trimmed == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(1);
	}

	strncpy(trimmed, input, len);
	trimmed[len] = '\0';

	return trimmed;
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
		return 0;
	}

	char *token = strtok(substring, delimiter);
	while (token != NULL) {
		if (! strstr(token, pattern)) {
			free(substring);
			return 1;
		}
		token = strtok(NULL, delimiter);
	}

	free(substring);
	return 0;
}

void matches_end_statement(char *line, const char *end_statements[]) {
	for (size_t i = 0; end_statements[i] != NULL; i++) {
		if (strcmp(line, end_statements[i]) == 0) {
			print_issue("Empty line before 'end' statement", line);
			return;
		}
	}
}

void check_general_issues(char *line) {
	if (strstr(line, "\t ")) {
		print_issue("Line contains mixed indentation", line);
	}

	line = trim_whitespace(line);

	if (line[strlen(line) - 1] == ' ' || line[strlen(line) - 1] == '\t') {
		print_issue("Line ends with whitespace", line);
	}

	if ( !ignore_consecutive_spaces && strstr(line, "  ")) {
		print_issue("Line contains two or more consecutive spaces", line);
	}
}

void check_brs_issues(char *line) {
	line = trim_whitespace(line);

	if (strncmp(line, "if", 2) == 0 && !strstr(line, " then")) {
		print_issue("Missing 'then' in 'if' statement", line);
	} else if (strncmp(line, "function", 8) == 0 || strncmp(line, "sub", 3) == 0) {
		if (line[0] == 'f') {
			if (line[strlen(line) - 1] == ')') print_issue("Function has no return type", line);
		}

		if (line[strlen(line) - 2] != '(') {
			int start = find_first_char_position(line, '(') + 1;
			int end = find_last_char_position(line, ')');
			if (find_pattern_in_substring(line, start, end, ",", " as ")) {
				print_issue("Function has no input type", line);
			}
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
		if (previous_was_empty) {
			print_issue("Warning: Multiple empty lines detected", line_content);
		}
		previous_was_empty = 1;
		return;
	}

	check_general_issues(line_content);
	if (strstr(current_file, ".brs")) check_brs_issues(line_content);
	if (current_line_reported) printf("\n");
	previous_was_empty = 0;
}

void validate_file(const char *file_path) {
	FILE *file = fopen(file_path, "r");
	if (!file) {
		perror("Failed to open file");
		return;
	}

	snprintf(current_file, sizeof(current_file), "%s", file_path);

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
		return;
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
			validate_file(full_path);
		}
	}

	closedir(dir);
}

int main(int argc, char *argv[]) {
	int opt;
	char *ignore_list = NULL;
	const char *path;

	while ((opt = getopt(argc, argv, "p:i:")) != -1) {
		switch (opt) {
			case 'p': path = optarg; break;
			case 'i': ignore_list = optarg; break;
			default: fprintf(stderr, "Invalid option.\n"); return 1;
		}
	}

	if (ignore_list) {
		char *token = strtok(ignore_list, ",");
		while (token != NULL) {
			if (strcmp(token, "consecutive_spaces") == 0) {
				ignore_consecutive_spaces = 1;
			}
			token = strtok(NULL, ",");
		}
	}

		printf("Ignoring: %s\n", ignore_list);

	struct stat statbuf;
	if (stat(path, &statbuf) == 0) {
		if (S_ISDIR(statbuf.st_mode)) {
			validate_directory(path);
		} else if (S_ISREG(statbuf.st_mode)) {
			validate_file(path);
		} else {
			fprintf(stderr, "The path is neither a regular file nor a directory: %s\n", path);
			return 1;
		}
	} else {
		perror("stat failed");
		return 1;
	}

	return 0;
}
