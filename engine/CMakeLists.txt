cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POLICY_DEFAULT_CMP0072 NEW)

set(ROOT_DIR ..)
set(ENGINE_DIR .)

###### Generic Includes ######
include(include/Spark)

###### Third Party ######
include(ThirdParty)

setup_target_common(Spark TRUE "${CMAKE_CURRENT_SOURCE_DIR}/unity.conf")
target_precompile_headers(Spark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:spark.h>)

###### Linker ######
target_link_libraries(Spark OpenFBX bc7enc)
if (NOT ${DIST_BUILD})
    target_link_libraries(Spark easy_profiler)
endif()

if (WIN32)
    target_link_libraries(Spark Dwmapi)

    if (${USE_EDITOR})
        target_link_libraries(Spark Editor)
    endif()
endif()

###### Platform Linker ######
if (${PLATFORM} MATCHES Windows)
    include(windows/WindowsLinker)
elseif (${PLATFORM} MATCHES Linux)
    include(linux/LinuxLinker)
elseif (${PLATFORM} MATCHES Mac)
    include(mac/MacLinker)
endif ()

#target_compile_options(Spark PUBLIC -fsanitize=address)
#target_link_options(Spark PUBLIC -fsanitize=address)
