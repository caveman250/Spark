#include "spark.h"
#include "Log.h"

#include <cstdio>

#define BEGIN_YELLOW printf("\033[0;33m");
#define BEGIN_WHITE printf("\033[0;37m");
#define BEGIN_RED printf("\033[0;31m");
#define BEGIN_FATAL printf("\033[0;41m");

#define END_COLOUR printf("\033[0m\n");

namespace se
{
	void Log::LogInternal(LogLevel level, const char* fmt, ...)
	{
		char buffer[1024];
		va_list args;
        va_start(args, fmt);
		vsnprintf(buffer, 1024, fmt, args);
		printf("%s", buffer);
		fflush(stdout);

		va_end(args);
	}

	void Log::Info(const char* fmt, ...)
	{
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, 1024, fmt, args);
		LogInternal(LogLevel::Info, "[Info] %s\n", (char*)&buffer);

		va_end(args);

	}

	void Log::Warn(const char* fmt, ...)
	{
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, 1024, fmt, args);
		BEGIN_YELLOW
		LogInternal(LogLevel::Warn, "[Warn] %s",(char*)&buffer);
		END_COLOUR

		va_end(args);
	}

	void Log::Error(const char* fmt, ...)
	{
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, 1024, fmt, args);

		BEGIN_RED
		LogInternal(LogLevel::Error, "[Error] %s", (char*)&buffer);
		END_COLOUR

		va_end(args);
	}

	void Log::Fatal(const char* fmt, ...)
	{
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, 1024, fmt, args);

		BEGIN_FATAL
		LogInternal(LogLevel::Fatal, "[Fatal] %s", (char*)&buffer);
		END_COLOUR

		va_end(args);
		__debugbreak();
		exit(0);
	}
}
