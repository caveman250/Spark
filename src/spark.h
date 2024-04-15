#pragma once

#include <stdio.h>
#include <vector>
#include <functional>
#include <string>
#include <ios>
#include <fstream>
#include <sstream>
#include <format>
#include <map>
#include <array>
#include <optional>

#include "engine/math/math.h"

#if SPARK_PLATFORM_WINDOWS
#include <windows.h>
#include <crtdbg.h>
#include <dwmapi.h>
#endif

#if SPARK_RELEASE
#define SPARK_ASSERT(expr, ...) do {} while (0)
#elif SPARK_DEBUG
#define SPARK_ASSERT(expr, ...) \
do { \
    if (!(expr))\
    {\
        char msg_buf[1024]; \
        SPARK_ASSERT_MESSAGE(msg_buf, __VA_ARGS__)\
        char buf[1024]; \
        snprintf(buf, 1024, "%s\n\nMessage: %s\n", #expr, (const char*)&msg_buf); \
        printf("\033[0;41mAssertion failed: %s - at %s:%i\033[0m\n\n", (const char*)&buf, __FILE__, __LINE__); \
        fflush(stdout);\
        bool assertResult = _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, "Spark Application", buf); \
        if (assertResult == 0)\
        {\
            break; \
        }\
        else\
        {\
            __debugbreak(); \
        }\
    }\
} while (0)
#else
#define SPARK_ASSERT(...) do {} while(0)
#endif

#if SPARK_DIST
#define SPARK_VERIFY(expr, ...) (!!(expr))
#else
#define SPARK_VERIFY(expr, ...) (!(expr) ? (::std::invoke([&](bool result) -> bool  { SPARK_ASSERT(expr, __VA_ARGS__); return result; }, !!(expr))), false : true)
#endif

#define SPARK_CAT_III(_, expr) expr
#define SPARK_CAT_II(a, b) SPARK_CAT_III(~, a ## b)
#define SPARK_CAT(a, b) SPARK_CAT_II(a, b)

#define SPARK_VARG_COUNT(...) SPARK_EXPAND_ARGS(SPARK_AUGMENT_ARGS(__VA_ARGS__), SPARK_VARG_COUNT_HELPER())
#define SPARK_VARG_COUNT_01N(...) SPARK_EXPAND_ARGS(SPARK_AUGMENT_ARGS(__VA_ARGS__), SPARK_VARG_COUNT_01N_HELPER())

#define SPARK_VARG_COUNT_HELPER() 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define SPARK_VARG_COUNT_01N_HELPER() N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, 1, 0

#define SPARK_EXPAND(x) x

#define SPARK_AUGMENT_ARGS(...) unused, __VA_ARGS__
#define SPARK_EXPAND_ARGS(...) SPARK_EXPAND(SPARK_GETARGCOUNT(__VA_ARGS__))
#define SPARK_GETARGCOUNT(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, count, ...) count

#define SPARK_ASSERT_MESSAGE_0(msg_buf, ...) snprintf(msg_buf, 1024, " ");
#define SPARK_ASSERT_MESSAGE_1(msg_buf, ...) snprintf(msg_buf, 1024, __VA_ARGS__);
#define SPARK_ASSERT_MESSAGE_N(msg_buf, msg, ...) snprintf(msg_buf, 1024, msg, __VA_ARGS__);
#define SPARK_ASSERT_MESSAGE(msg_buf, ...) SPARK_CAT(SPARK_ASSERT_MESSAGE_, SPARK_VARG_COUNT_01N(__VA_ARGS__))(msg_buf, __VA_ARGS__)