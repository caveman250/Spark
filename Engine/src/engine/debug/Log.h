#pragma once
#include "signal.h"

namespace se::debug
{
    class Log
    {
    public:
        template <class... _Types>
        static void Info(const std::string_view fmt, _Types&&... args);

        template <class... _Types>
        static void Warn(const std::string_view fmt, _Types&&... args);

        template <class... _Types>
        static void Error(const std::string_view fmt, _Types&&... args);

        template <class... _Types>
        static void Fatal(const std::string_view fmt, _Types&&... args);
    private:
        template <class... _Types>
        static void LogInternal(const std::string_view& _Fmt, const std::format_args& _Args);
    };

#define BEGIN_YELLOW printf("\033[0;33m");
#define BEGIN_WHITE printf("\033[0;37m");
#define BEGIN_RED printf("\033[0;31m");
#define BEGIN_FATAL printf("\033[0;41m");
#define END_COLOUR printf("\033[0m\n");


    template<class ... _Types>
    void Log::Info(const std::string_view fmt, _Types &&...args)
    {
        printf("[Info]: ");
        LogInternal(fmt, std::make_format_args(args...));
    }

    template<class ... _Types>
    void Log::Warn(const std::string_view fmt, _Types &&...args)
    {
        BEGIN_YELLOW
        printf("[Warn]: ");
        LogInternal(fmt, std::make_format_args(args...));
        END_COLOUR
    }

    template<class ... _Types>
    void Log::Error(const std::string_view fmt, _Types &&...args)
    {
        BEGIN_RED
        printf("[Error]: ");
        LogInternal(fmt, std::make_format_args(args...));
        END_COLOUR
    }

    template<class ... _Types>
    void Log::Fatal(const std::string_view fmt, _Types &&...args)
    {
        BEGIN_FATAL
        printf("[Fatal]: ");
        LogInternal(fmt, std::make_format_args(args...));
        END_COLOUR

#if !SPARK_DIST
#if SPARK_PLATFORM_WINDOWS
        __debugbreak();
#else
        raise(SIGTRAP);
#endif
#endif
        exit(-1);
    }

    template<class ... _Types>
    void Log::LogInternal(const std::string_view &_Fmt, const std::format_args &_Args)
    {
        std::string str;
        std::vformat_to(std::back_insert_iterator{str}, _Fmt, _Args);

        printf("%s\n", str.c_str());
        fflush(stdout);
    }
}