#pragma once

#include <cstdio>
#include <vector>
#include <functional>
#include <string>
#include <ios>
#include <fstream>
#include <sstream>
#include <format>
#include <map>
#include <unordered_set>
#include <array>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>
#include <ranges>
#include <utility>
#include <regex>
#include <thread>
#include <chrono>
#include <execution>
#include <cfloat>
#include <mutex>
#include <variant>
#include <variant>

#include "json.hpp"

#if SPARK_PLATFORM_WINDOWS
#include "windows.h"
#include "windowsx.h"
#include "crtdbg.h"
#include "dwmapi.h"

#undef min
#undef max
#undef LoadImage
#undef CreateWindow
#undef DELETE
#elif SPARK_PLATFORM_LINUX
#include "SDL2/SDL.h"
#undef linuxt
#endif

#if !SPARK_DIST
#if SPARK_PLATFORM_WINDOWS
#if SPARK_DEBUG
#define SPARK_ASSERT(expr, ...) \
do { \
    if (!(expr))\
    {\
        std::string userMsg = SPARK_ASSERT_MESSAGE(__VA_ARGS__)\
        std::string assertMsg = std::format("{0}\n\nMessage: {1}\n", #expr, userMsg); \
        printf(std::format("\033[0;41mAssertion failed: {0}at {1}:{2}\033[0m\n\n", assertMsg,  __FILE__, __LINE__).c_str()); \
        fflush(stdout);         \
        bool assertResult = _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, "Spark Application", assertMsg.c_str()); \
        if (assertResult == 0)\
        {\
            break; \
        }\
        else                    \
        {\
            __debugbreak(); \
        }\
    }\
} while (0)
#else
#define SPARK_ASSERT(expr, ...) \
do { \
    if (!(expr))\
    {\
        std::string userMsg = SPARK_ASSERT_MESSAGE(__VA_ARGS__)\
        std::string assertMsg = std::format("{0}\n\nMessage: {1}\n", #expr, userMsg); \
        printf(std::format("\033[0;41mAssertion failed: {0}at {1}:{2}\033[0m\n\n", assertMsg,  __FILE__, __LINE__).c_str()); \
        fflush(stdout);         \
        __debugbreak(); \
    }\
} while (0)
#endif
#elif SPARK_PLATFORM_LINUX
#include "csignal"
#define SPARK_ASSERT(expr, ...)\
do { \
    if (!(expr))\
    {\
        std::string userMsg = SPARK_ASSERT_MESSAGE(__VA_ARGS__)\
        std::string assertMsg = std::format("{0}\n\nMessage: {1}\n", #expr, userMsg); \
        printf("%s", std::format("\033[;41mAssertion failed: {0}at {1}:{2}\033[0m\n\n", assertMsg,  __FILE__, __LINE__).c_str()); \
        fflush(stdout);\
        raise(SIGTRAP);\
    }\
} while (0)
#elif SPARK_PLATFORM_MAC
#define SPARK_ASSERT(expr, ...)\
do { \
if (!(expr))\
{\
std::string userMsg = SPARK_ASSERT_MESSAGE(__VA_ARGS__)\
std::string assertMsg = std::format("{0}\n\nMessage: {1}\n", #expr, userMsg); \
printf("%s", std::format("\033[;41mAssertion failed: {0}at {1}:{2}\033[0m\n\n", assertMsg,  __FILE__, __LINE__).c_str()); \
fflush(stdout);\
__builtin_debugtrap();\
}\
} while (0)
#endif
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

#define SPARK_VARG_COUNT_01N(...) SPARK_EXPAND_ARGS(SPARK_AUGMENT_ARGS(__VA_ARGS__), SPARK_VARG_COUNT_01N_HELPER())
#define SPARK_VARG_COUNT_01N_HELPER() N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, 1, 0

#define SPARK_EXPAND(x) x

#define SPARK_AUGMENT_ARGS(...) unused, __VA_ARGS__
#define SPARK_EXPAND_ARGS(...) SPARK_EXPAND(SPARK_GETARGCOUNT(__VA_ARGS__))
#define SPARK_GETARGCOUNT(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, count, ...) count

#define SPARK_ASSERT_MESSAGE_0(...) " ";
#define SPARK_ASSERT_MESSAGE_1(...) std::string(__VA_ARGS__);
#define SPARK_ASSERT_MESSAGE_N(msg, ...) std::format(msg, __VA_ARGS__);
#define SPARK_ASSERT_MESSAGE(...) SPARK_CAT(SPARK_ASSERT_MESSAGE_, SPARK_VARG_COUNT_01N(__VA_ARGS__))(__VA_ARGS__)

#if SPARK_PLATFORM_WINDOWS
#define PLACEHOLDER(i) std::_Ph<i>
#elif SPARK_PLATFORM_MAC
#define PLACEHOLDER(i) std::placeholders::__ph<i>
#else
#define PLACEHOLDER(i) std::_Placeholder<i>
#endif

inline void hash_combine(std::size_t&) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, rest...);
}

#include "engine/debug/Log.h"
#include "engine/ecs/ecs_fwd.h"
#include "engine/reflect/Reflect_fwd.h"