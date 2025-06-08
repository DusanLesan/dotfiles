#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <json-c/json.h>

#define MAX_SIZE 1024

char* get_password(const char *prompt) {
	int pipefd[2];
	if (pipe(pipefd) == -1) return NULL;

	pid_t pid = fork();
	if (pid == -1) {
		close(pipefd[0]); close(pipefd[1]);
		return NULL;
	}

	if (pid == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		char *const argv[] = {"dmenu", "-nb", "#A54242", "-P", "-p", (char*)prompt, NULL};
		execvp("dmenu", argv);
		_exit(1);
	}

	close(pipefd[1]);
	FILE *fp = fdopen(pipefd[0], "r");
	if (!fp) {
		close(pipefd[0]);
		return NULL;
	}

	char *response = malloc(MAX_SIZE);
	if (!response || !fgets(response, MAX_SIZE, fp)) {
		free(response);
		fclose(fp);
		waitpid(pid, NULL, 0);
		return NULL;
	}

	fclose(fp);
	waitpid(pid, NULL, 0);

	size_t len = strlen(response);
	if (len > 0 && response[len-1] == '\n')
		response[len-1] = '\0';

	return response;
}

char* get_json_string(json_object *obj, const char *key, const char *default_val) {
	json_object *field;
	if (json_object_object_get_ex(obj, key, &field)) {
		const char *str = json_object_get_string(field);
		return str ? strdup(str) : (default_val ? strdup(default_val) : NULL);
	}
	return default_val ? strdup(default_val) : NULL;
}

int is_password_request(json_object *obj) {
	json_object *action;
	if (json_object_object_get_ex(obj, "action", &action)) {
		const char *str = json_object_get_string(action);
		return str && strcmp(str, "request password") == 0;
	}
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2 || strcmp(argv[1], "_CALLED_INTERNALLY") != 0) {
		char cmd[MAX_SIZE];
		snprintf(cmd, sizeof(cmd), "cmd-polkit-agent -s -c \"%s _CALLED_INTERNALLY\"", argv[0]);
		execl("/bin/sh", "sh", "-c", cmd, NULL);
		exit(1);
	}

	char line[MAX_SIZE];
	while (fgets(line, sizeof(line), stdin)) {
		size_t len = strlen(line);
		if (len > 0 && line[len-1] == '\n')
			line[len-1] = '\0';

		json_object *obj = json_tokener_parse(line);
		if (!obj) continue;

		if (is_password_request(obj)) {
			char *prompt = get_json_string(obj, "prompt", "Password:");
			char *message = get_json_string(obj, "message", "Authentication required");

			char full_prompt[MAX_SIZE];
			snprintf(full_prompt, sizeof(full_prompt), "%s. %s", message, prompt);

			char *password = get_password(full_prompt);

			if (!password || strlen(password) == 0) {
				printf("{\"action\":\"cancel\"}\n");
			} else {
				printf("{\"action\":\"authenticate\",\"password\":\"%s\"}\n", password);
			}
			fflush(stdout);

			free(prompt);
			free(message);
			free(password);
		}

		json_object_put(obj);
	}

	return 0;
}
