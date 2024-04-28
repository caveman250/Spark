###### Platform Defs ######
if (WIN32)
    set(PLATFORM Windows)
    add_definitions(-DSPARK_PLATFORM_WINDOWS=1)
elseif (UNIX)
    if(APPLE)
        set(PLATFORM Mac)
        add_definitions(-DSPARK_PLATFORM_MAC=1)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES Android)
        set(PLATFORM Android)
        add_definitions(-DSPARK_PLATFORM_ANDROID=1)
    else()
        set(PLATFORM Linux)
        add_definitions(-DSPARK_PLATFORM_LINUX=1)
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