cmake_minimum_required(VERSION 3.28)
project(Spark)

###### Platform Defs ######
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

###### Config Defs ######
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DSPARK_DEBUG=1)
endif()

if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    add_definitions(-DSPARK_RELEASE=1)
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DSPARK_DIST=1)
endif()

###### Projects ######
add_subdirectory(Engine)
add_subdirectory(Editor)