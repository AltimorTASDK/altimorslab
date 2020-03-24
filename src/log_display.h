#include <stdio.h>

#define LogDisplay_Printf(format, ...) \
	{ \
		char buf[256]; \
		sprintf(buf, format, __VA_ARGS__); \
		LogDisplay_Print(buf); \
	}

void LogDisplay_Print(const char *message);