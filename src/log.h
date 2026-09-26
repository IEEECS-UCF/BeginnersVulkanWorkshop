#ifndef LOG_H
#define LOG_H

typedef enum {
	LOG_OK,
	LOG_DEBG,
	LOG_INFO,
	LOG_WARN,
	LOG_FAIL
} LOG_LEVEL;

void _logger_log(LOG_LEVEL level, const char* fmt, ...);

#define log_ok(...)   _logger_log(LOG_OK, __VA_ARGS__)
#define log_debg(...) _logger_log(LOG_DEBG, __VA_ARGS__)
#define log_info(...) _logger_log(LOG_INFO, __VA_ARGS__)
#define log_warn(...) _logger_log(LOG_WARN, __VA_ARGS__)
#define log_fail(...) _logger_log(LOG_FAIL, __VA_ARGS__)

#endif
