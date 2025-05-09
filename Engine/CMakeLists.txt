cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POLICY_DEFAULT_CMP0072 NEW)

set(ROOT_DIR ..)
set(ENGINE_DIR .)

###### Generic Includes ######
include(${ROOT_DIR}/CMake/include/Spark.cmake)

###### Third Party ######
include(${ROOT_DIR}/CMake/ThirdParty.cmake)

setup_target_common(Spark TRUE "${ROOT_DIR}/Engine/unity.conf")
target_precompile_headers(Spark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:spark.h>)

###### Linker ######
target_link_libraries(Spark OpenFBX bc7enc)
if (WIN32)
    target_link_libraries(Spark Dwmapi)
endif()

###### Platform Linker ######
if (${PLATFORM} MATCHES Windows)
    include(${ROOT_DIR}/CMake/windows/WindowsLinker.cmake)
elseif (${PLATFORM} MATCHES Linux)
    include(${ROOT_DIR}/CMake/linux/LinuxLinker.cmake)
elseif (${PLATFORM} MATCHES Mac)
    include(${ROOT_DIR}/CMake/mac/MacLinker.cmake)
endif ()

#target_compile_options(Spark PUBLIC -fsanitize=address)
#target_link_options(Spark PUBLIC -fsanitize=address)
