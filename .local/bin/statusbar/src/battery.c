#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void setbacklight(char* action) {
	char cmd[16] = "sudo backlight ";
	strncat(cmd, action, 1);
	system(cmd);
}

int main() {
	char* button = getenv("BLOCK_BUTTON");
	if(!button || !*button)
		button = "0";
	
	if (atoi(button) == 4)
		setbacklight("+");
	else if (atoi(button) == 5)
		setbacklight("-");

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
