cmake_minimum_required(VERSION 3.28)
project(Spark)

set(CMAKE_CXX_STANDARD 23)

if (WIN32)
    set(PLATFORM Windows)
    add_definitions(-DSPARK_PLATFORM_WINDOWS=1)
elseif (UNIX)
    if(APPLE)
        set(PLATFORM Mac)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES Android)
        set(PLATFORM Android)
    else()
        set(PLATFORM Linux)
    endif()
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DSPARK_DEBUG=1)
endif()

if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    add_definitions(-DSPARK_RELEASE=1)
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DSPARK_DIST=1)
endif()

find_package(OpenGL REQUIRED)

include_directories(src)


file(GLOB PLATFORM_SHARED_CODE src/platform/*.cpp src/platform/*.h)
if (${PLATFORM} MATCHES Windows)
    file(GLOB_RECURSE PLATFORM_CODE src/platform/windows/*.cpp src/platform/windows/*.h)
endif()
file(GLOB_RECURSE ENGINE_CODE src/engine/*.cpp src/engine/*.h)

message(STATUS ${PLATFORM_SHARED_CODE})
message(STATUS ${PLATFORM_CODE})
message(STATUS ${ENGINE_CODE})

add_executable(Spark ${PLATFORM_SHARED_CODE} ${PLATFORM_CODE} ${ENGINE_CODE} src/main.cpp)

target_precompile_headers(Spark PUBLIC "src/spark.h")

target_link_libraries(Spark ${OPENGL_LIBRARY})
