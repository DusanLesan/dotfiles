#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

void setbacklight(char* action) {
	char cmd[16] = "sudo backlight ";
	strncat(cmd, action, 1);
	system(cmd);
}

int main(void) {
	char *btn = getenv("BLOCK_BUTTON");
	if (btn != NULL) {
		switch (atoi(btn)) {
			case 4: setbacklight("+"); break;
			case 5: setbacklight("-"); break;
		}
	}

	char cap[4], stat[16];
	char* icon = "󰁹";
	int found = 0;
	DIR *d = opendir("/sys/class/power_supply/");
	if (!d) return 1;

	struct dirent *e;
	while ((e = readdir(d)) != NULL) {
		if (strncmp(e->d_name, "BAT", 3) == 0) {
			char path[256];
			snprintf(path, sizeof(path), "/sys/class/power_supply/%s", e->d_name);

			char cap_file[256], stat_file[256];
			snprintf(cap_file, sizeof(cap_file), "%s/capacity", path);
			snprintf(stat_file, sizeof(stat_file), "%s/status", path);

			FILE *fc = fopen(cap_file, "r");
			if (fc) {
				fscanf(fc, "%[^\n]", cap);
				fclose(fc);

				FILE *fs = fopen(stat_file, "r");
				if (fs) {
					fscanf(fs, "%[^\n]", stat);
					fclose(fs);

					if (strcmp(stat, "Discharging") == 0) icon = "󱟟";
					else if (strcmp(stat, "Charging") == 0) icon = "󰂄";
					found = 1;
					break;
				}
			}
		}
	}

	closedir(d);
	if (!found) {
		return 1;
	}

	printf("%s%s\n", icon, cap);
	return 0;
}
