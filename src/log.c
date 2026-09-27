#include "log.h"

#include <stdarg.h>
#include <stdio.h>

#define LOG_LEVEL_CNT 5
#define BOLD  "\x1b[1m"
#define RESET "\x1b[0m" // Resets to whatever the default color and styling of the terminal is

char* colors[LOG_LEVEL_CNT] = {
	BOLD"\x1b[32m", 	 // Bold green for 'ok'
	"\x1b[38;5;248m",	 // Greyish/white for debug
	"\x1b[38;5;147m",	 // Light blue for info
	"\x1b[38;5;214m",	 // Yellow/orange for a warning
	BOLD"\x1b[38;5;160m" // Bold red for failure
};

// total hackjob but it works
void _logger_log(LOG_LEVEL level, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	fputs(colors[level], stdout);
	vfprintf(stdout, fmt, args);
	fputs(RESET, stdout);

	va_end(args);
	return;
}
