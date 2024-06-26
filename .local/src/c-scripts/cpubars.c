#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_MAX 256
#define MAX_CPU 31

void sysact(void) {
	pid_t child_pid = fork();
	if (child_pid == 0) {
		char *args[] = {"/bin/alacritty", "--class", "floating", "-e", "sysact", NULL};
		setenv("WINIT_X11_SCALE_FACTOR", "1", 1);
		setenv("USE_TERM", "true", 1);
		execv(args[0], args);
	}
}

int read_fields(char buffer[], unsigned long long int *fields) {
	return sscanf(buffer, "c%*s %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
			&fields[0], &fields[1], &fields[2], &fields[3], &fields[4],
			&fields[5], &fields[6], &fields[7], &fields[8], &fields[9]);
}

int parse_cpu(void) {
	FILE *statFile, *cacheFile;
	unsigned long long int fields[10], total_tick[MAX_CPU], total_tick_old[MAX_CPU], idle[MAX_CPU], idle_old[MAX_CPU], del_total_tick[MAX_CPU], del_idle[MAX_CPU];
	int i, cpus = 0;
	double percent_usage;
	char buffer[BUF_MAX];
	char *bars[] = {"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};

	statFile = fopen("/proc/stat", "r");
	if (statFile == NULL) {
		perror("Error");
		return 1;
	}

	cacheFile = fopen("/tmp/cpubarscache", "r+");
	if (cacheFile == NULL) {
		cacheFile = fopen("/tmp/cpubarscache", "w+");
		if (cacheFile == NULL) {
			perror("Error opening file");
			return 1;
		}
	} else {
		while (fgets(buffer, sizeof(buffer), cacheFile) != NULL) {
			read_fields(buffer, fields);
			idle_old[cpus] = fields[1];
			total_tick_old[cpus] = fields[0];
			cpus++;
		}
		fseek(cacheFile, 0, SEEK_SET);
		cpus = 0;
	}

	while (fgets(buffer, sizeof(buffer), statFile) != NULL && read_fields(buffer, fields) == 10) {
		for (i = 0, total_tick[cpus] = 0; i < 10; i++) {
			total_tick[cpus] += fields[i];
		}
		idle[cpus] = fields[3];

		fprintf(cacheFile, "cpu%d ", cpus);
		fprintf(cacheFile, "%llu ", total_tick[cpus]);
		fprintf(cacheFile, "%llu\n", idle[cpus]);

		del_total_tick[cpus] = total_tick[cpus] - total_tick_old[cpus];
		del_idle[cpus] = idle[cpus] - idle_old[cpus];
		percent_usage = ((del_total_tick[cpus] - del_idle[cpus]) / (double)del_total_tick[cpus]) * 100;
		i = percent_usage / 12.6;
		if (cpus != 0 && i < 8 && i > -1)
			printf("%s", bars[i]);

		cpus++;
	}
	printf("\n");

	fclose(cacheFile);
	fclose(statFile);
	return 0;
}

int main(void) {
	char *button = getenv("BLOCK_BUTTON");
	if (button != NULL)
		if (atoi(button) == 1)
			sysact();

	parse_cpu();
}
