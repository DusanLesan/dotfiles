#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void displayhogs() {
	size_t size = 0;
	char *buffer = NULL;
	FILE *fp = popen("ps axch -o cmd:15,%mem --sort=-%mem", "r");

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	buffer = malloc((size + 1) * sizeof(*buffer));

	rewind(fp);
	fread(buffer, size, 1, fp);
	buffer[size] = '\0';

	char *args[] = {"/bin/notify-send", "Memmory hogs:", buffer, NULL};
	execv(args[0], args);
	pclose(fp);
}

void openhtop() {
	char *args[] = {"/bin/alacritty", "-e", "htop", NULL};
	execv(args[0], args);
}

int main() {
	char* button = getenv("BLOCK_BUTTON");
	if(!button || !*button)
		button = "0";

	if (atoi(button) == 1)
		displayhogs();
	else if (atoi(button) == 3)
		openhtop();

	FILE *meminfo = fopen("/proc/meminfo", "r");
	char line[256];
	int total, free, available, buffers, cached, sReclaimable, shmem;
	while(fgets(line, sizeof(line), meminfo)) {
		sscanf(line, "MemTotal: %d kB", &total);
		sscanf(line, "MemFree: %d kB", &free);
		sscanf(line, "MemAvailable: %d kB", &available);
		sscanf(line, "Buffers: %d kB", &buffers);
		sscanf(line, "Cached: %d kB", &cached);
		sscanf(line, "SReclaimable: %d kB", &sReclaimable);
		sscanf(line, "Shmem: %d kB", &shmem);
	}
	fclose(meminfo);
	double used = total - free - buffers - cached - sReclaimable;
	printf("%.2f\n", used / 1024 / 1024);
	return 0;
}
