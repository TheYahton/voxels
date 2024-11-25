#include <stdio.h>
#include <time.h>

static void formatted_time(char *buffer) {
	time_t timer = time(NULL);
	struct tm *tm_info = localtime(&timer);

	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
}

void info(const char *text) {
	char buffer[26];
	formatted_time(buffer);
	printf("[%s] INFO: %s\n", buffer, text);
}

void warning(const char *text) {
	char buffer[26];
	formatted_time(buffer);
	printf("[%s] WARNING: %s\n", buffer, text);
}

void error(const char *text) {
	char buffer[26];
	formatted_time(buffer);
	printf("[%s] ERROR: %s\n", buffer, text);
}
