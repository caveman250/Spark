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
message("Configuring Engine source files...")
get_filename_component(ABSOLUTE_PATH ${ROOT_DIR}/Build ABSOLUTE)
execute_process(COMMAND ${PYTHON_EXE} UnityBatcher.py ${ROOT_DIR}/Engine/unity.conf ${PLATFORM} WORKING_DIRECTORY ${ABSOLUTE_PATH} RESULT_VARIABLE rv)
file(GLOB SOURCE ./unity_generated/*.cpp ./sht_generated/*.cpp)
if (${PLATFORM} MATCHES Mac)
    file(GLOB_RECURSE OBJC_SOURCE ./unity_generated/*.mm)
endif()

message("Adding library: Spark\n")
file(GLOB_RECURSE NON_UNITY_SOURCE src/*.cpp src/*.h src/*.mm)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NON_UNITY_SOURCE})
if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(SOURCE ${NON_UNITY_SOURCE} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")
    add_library(Spark ${SOURCE} ${OBJC_SOURCE} ${NON_UNITY_SOURCE})
else()
    add_library(Spark ${SOURCE} ${OBJC_SOURCE})
endif()
setup_compile_options(Spark)
set_target_properties(Spark PROPERTIES EXCLUDE_FROM_ALL TRUE)

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
if (NOT CMAKE_GENERATOR MATCHES "Visual Studio")
    add_custom_target("Spark_Source_Files" SOURCES ${NON_UNITY_SOURCE})
endif()

if(NOT CMAKE_CONFIGURATION_TYPES)
    add_dependencies(Spark ${REGEN_CMAKE_TARGET})
endif()
