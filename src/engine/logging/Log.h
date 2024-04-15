#pragma once

namespace se
{
    enum class LogLevel;
    class Log
    {
    public:
        static void Info(const char* fmt, ...);
        static void Warn(const char* fmt, ...);
        static void Error(const char* fmt, ...);
        static void Fatal(const char* fmt, ...);
    private:
        static void LogInternal(LogLevel level, const char* fmt, ...);
    };

    enum class LogLevel
    {
        Info,
        Warn,
        Error,
        Fatal
    };
}