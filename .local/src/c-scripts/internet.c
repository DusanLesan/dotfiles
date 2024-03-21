#include <string.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void nmtui(void) {
	pid_t child_pid = fork();
	if (child_pid == 0) {
		char *args[] = {"/bin/alacritty", "--class", "floating", "-e", "nmtui-connect", NULL};
		setenv("WINIT_X11_SCALE_FACTOR", "1", 1);
		execv(args[0], args);
	}
}

int main(int argc, char *argv[]) {
	char* button = getenv("BLOCK_BUTTON");
	if(button != NULL)
		if (atoi(button) == 1)
			nmtui();

	struct ifaddrs *ifaddr;
	int family, s;
	int w = 0, e = 0, h = 0, v = 0;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return 1;
	}

	for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;

		family = ifa->ifa_addr->sa_family;
		if (family == AF_INET) {
			s = getnameinfo(ifa->ifa_addr,
				(family == AF_INET) ? sizeof(struct sockaddr_in):
				sizeof(struct sockaddr_in6),
				host, NI_MAXHOST,
				NULL, 0, NI_NUMERICHOST);

			switch (ifa->ifa_name[0]) {
				case 'w':
					w = 1;
					break;
				case 'a':
					h = 1;
					break;
				case 'e':
					e = 1;
					break;
				case 't':
					v = 1;
					break;
			}
		}
	}

	char output[24] = "";
	if (e == 1) strcat(output, " 󰈀");
	if (h == 1) strcat(output, " 󰀃");
	if (w == 1) strcat(output, " 󰖩");
	if (v == 1) strcat(output, " 󰻌");
	if (strlen(output) <= 0) strcat(output, " ");

	printf("%s\n", &output[1]);

	freeifaddrs(ifaddr);
	return 0;
}
