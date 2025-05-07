#include "log.h"
#include "logger.h"

#include "color.h"
#include "system.h"

#include <atomic>
#include <cstdio>
#include <memory>

#if defined(CONF_FAMILY_WINDOWS)
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#if defined(CONF_PLATFORM_ANDROID)
#include <android/log.h>
#endif

void log_log(LEVEL level, const char *sys, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	log_log_v(level, sys, fmt, args);
	va_end(args);
}

void log_log_v(LEVEL level, const char *sys, const char *fmt, va_list args)
{
	char aLine[2048];
	vsnprintf(aLine, sizeof(aLine), fmt, args);

	char aTimestamp[512];
	polybob::str_timestamp_format(aTimestamp, sizeof(aTimestamp), FORMAT_SPACE);
	fprintf(level == LEVEL_ERROR ? stderr : stdout, "%s %c %s: %s\n", aTimestamp, "EWIDT"[level], sys, aLine);
}
