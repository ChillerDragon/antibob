#pragma once

#include <cstdarg>
#include <cstdint>

#ifdef __GNUC__
#define GNUC_ATTRIBUTE(x) __attribute__(x)
#else
#define GNUC_ATTRIBUTE(x)
#endif

enum LEVEL : char
{
	LEVEL_ERROR,
	LEVEL_WARN,
	LEVEL_INFO,
	LEVEL_DEBUG,
	LEVEL_TRACE,
};

#define log_error(sys, ...) log_log(LEVEL_ERROR, sys, __VA_ARGS__)
#define log_warn(sys, ...) log_log(LEVEL_WARN, sys, __VA_ARGS__)
#define log_info(sys, ...) log_log(LEVEL_INFO, sys, __VA_ARGS__)
#define log_debug(sys, ...) log_log(LEVEL_DEBUG, sys, __VA_ARGS__)
#define log_trace(sys, ...) log_log(LEVEL_TRACE, sys, __VA_ARGS__)

void log_log(LEVEL level, const char *sys, const char *fmt, ...)
	GNUC_ATTRIBUTE((format(printf, 3, 4)));

void log_log_v(LEVEL level, const char *sys, const char *fmt, va_list args)
	GNUC_ATTRIBUTE((format(printf, 3, 0)));
