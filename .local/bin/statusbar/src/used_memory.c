#include <stdio.h>

int main() {
	FILE *meminfo = fopen("/proc/meminfo", "r");
	char line[256];
	int total, free, available, buffers, cached, sReclaimable, shmem;
	while(fgets(line, sizeof(line), meminfo)) {
		if(sscanf(line, "MemTotal: %d kB", &total) == 1);
		else if(sscanf(line, "MemFree: %d kB", &free) == 1);
		else if(sscanf(line, "MemAvailable: %d kB", &available) == 1);
		else if(sscanf(line, "Buffers: %d kB", &buffers) == 1);
		else if(sscanf(line, "Cached: %d kB", &cached) == 1);
		else if(sscanf(line, "SReclaimable: %d kB", &sReclaimable) == 1);
		else if(sscanf(line, "Shmem: %d kB", &shmem) == 1);
	}
	fclose(meminfo);
	double used = total - free - buffers - cached - sReclaimable;
	printf("%.2f\n", used / 1024 / 1024);
	return 0;
}
