cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POLICY_DEFAULT_CMP0072 NEW)

set(ROOT_DIR ..)
set(ENGINE_DIR .)

###### Generic Includes ######
include(${ROOT_DIR}/CMake/include/Spark.cmake)

###### Third Party ######
include(${ROOT_DIR}/CMake/ThirdParty.cmake)

###### Generate Unity Files #######
message("Configuring Engine source files")
get_filename_component(ABSOLUTE_PATH ${ROOT_DIR}/Build ABSOLUTE)
execute_process(COMMAND ${PYTHON_EXE} UnityBatcher.py ${ROOT_DIR}/Engine/unity.conf ${PLATFORM} WORKING_DIRECTORY ${ABSOLUTE_PATH} RESULT_VARIABLE rv)
file(GLOB SOURCE ./unity_generated/*.cpp ./sht_generated/*.cpp)
if (${PLATFORM} MATCHES Mac)
    file(GLOB_RECURSE OBJC_SOURCE ./unity_generated/*.mm)
endif()

###### Add non Unity files for visualisation in VS and Xcode ######
file(GLOB_RECURSE NON_UNITY_SOURCE src/*.cpp src/*.h src/*.mm)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NON_UNITY_SOURCE})
add_custom_target("${target}_Source_Files" SOURCES ${NON_UNITY_SOURCE})

message("Adding library: Spark")
add_library(Spark ${SOURCE} ${OBJC_SOURCE})

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
target_precompile_headers(Spark PUBLIC $<$<COMPILE_LANGUAGE:CXX>:spark.h>)

#target_compile_options(Spark PUBLIC -fsanitize=address)
#target_link_options(Spark PUBLIC -fsanitize=address)

###### Add non Unity files for visualisation in VS and Xcode ######
file(GLOB_RECURSE NON_UNITY_SOURCE src/*.cpp src/*.h src/*.mm)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NON_UNITY_SOURCE})
add_custom_target("Spark_Source_Files" SOURCES ${NON_UNITY_SOURCE})

add_dependencies(Spark ${SHT_TARGET})
