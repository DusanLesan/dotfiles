#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void displayhogs(void) {
	size_t size = 0;
	char *buffer = NULL;
	FILE *fp = popen("ps axch -o cmd:15,%mem --sort=-%mem", "r");

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	buffer = malloc((size + 1) * sizeof(*buffer));

	rewind(fp);
	fread(buffer, size, 1, fp);
	buffer[size] = '\0';

	char *args[] = {"/bin/notify-send", "Memory hogs:", buffer, NULL};
	execv(args[0], args);
	pclose(fp);
}

void openhtop(void) {
	char *args[] = {"/bin/alacritty", "-e", "htop", NULL};
	execv(args[0], args);
}

void runbackground(void(*f)()) {
	pid_t child_pid = fork();
	if (child_pid == 0)
		f();
}

int main(void) {
	char* button = getenv("BLOCK_BUTTON");
	if (button != NULL) {
		switch (atoi(button)) {
			case 1:
				runbackground(displayhogs);
				break;
			case 3:
				runbackground(openhtop);
				break;
		}
	}

	FILE *meminfo = fopen("/proc/meminfo", "r");
	char line[64];
	int total = 0, free = 0, buffers = 0, cached = 0, sReclaimable = 0, shmem = 0;
	while (fgets(line, sizeof(line), meminfo)) {
		sscanf(line, "MemTotal: %d kB", &total);
		sscanf(line, "MemFree: %d kB", &free);
		sscanf(line, "Buffers: %d kB", &buffers);
		sscanf(line, "Cached: %d kB", &cached);
		sscanf(line, "SReclaimable: %d kB", &sReclaimable);
		sscanf(line, "Shmem: %d kB", &shmem);
	}
	fclose(meminfo);
	double used = total - free - buffers - cached - sReclaimable + shmem;
	printf("󰍛%.2f\n", used / 1024 / 1024);
	return 0;
}
