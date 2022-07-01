#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void setbacklight(int val) {
	FILE *fb = fopen("/sys/class/backlight/intel_backlight/brightness", "r+");
	char br[4];
	fscanf(fb, "%[^\n]", br);
	val = atoi(br) + val;
	if (val > 50)
		fprintf(fb, "%d", val);
	fclose(fb);
}

int main(void) {
	char* button = getenv("BLOCK_BUTTON");
	if (button != NULL) {
		switch (atoi(button)) {
			case 4:
				setbacklight(100);
				break;
			case 5:
				setbacklight(-100);
				break;
		}
	}

	FILE *fc = fopen("/sys/class/power_supply/BAT1/capacity", "r");
	char cap[4];
	fscanf(fc, "%[^\n]", cap);
	fclose(fc);
	
	FILE *fs = fopen("/sys/class/power_supply/BAT1/status", "r");
	char stat[16];
	fscanf(fs, "%[^\n]", stat);
	fclose(fs);

	char* icon = "";
	if(strcmp(stat, "Discharging") == 0)
		icon = "";
	else if (strcmp(stat, "Charging") == 0)
		icon = "";
	printf("%s%s\n", icon, cap);

	return 0;
}
