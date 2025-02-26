cmake_minimum_required(VERSION 3.28)
project(Spark)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POLICY_DEFAULT_CMP0072 NEW)

set(ROOT_DIR ..)
set(ENGINE_DIR .)

###### Generic Includes ######
include(${ROOT_DIR}/CMake/include/Spark.cmake)

###### Third Party ######
include(${ROOT_DIR}/CMake/ThirdParty.cmake)

###### Generate Unity Files #######
setup_source_files(Spark ../Engine/unity.conf true)

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

###### PCH ######
target_precompile_headers(Spark PUBLIC src/spark.h)

#target_compile_options(Spark PUBLIC -fsanitize=address)
#target_link_options(Spark PUBLIC -fsanitize=address)

###### Add non Unity files for visualisation in VS and Xcode ######
file(GLOB_RECURSE NON_UNITY_SOURCE src/*.cpp src/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NON_UNITY_SOURCE})
add_custom_target("Spark_Source_Files" SOURCES ${NON_UNITY_SOURCE})
