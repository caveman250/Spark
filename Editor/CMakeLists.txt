cmake_minimum_required(VERSION 3.28)
project(Spark)
set(CMAKE_CXX_STANDARD 23)

set(ROOT_DIR ..)
include(${ROOT_DIR}/CMake/Spark.cmake)
include(${ROOT_DIR}/CMake/ExternalTools.cmake)

###### Includes ######
include_directories(src)

###### Generate Unity Files #######

execute_process(COMMAND ${PYTHON_EXE} UnityBatcher.py ../Editor/unity.conf ${PLATFORM}
        WORKING_DIRECTORY ${ROOT_DIR}/Spark/Build/)

###### Source ######
file(GLOB SOURCE ./unity_generated/*.cpp)

###### Output ######
add_library(Editor ${SOURCE})

setup_spark_target(Editor OFF)

###### Add non Unity files for visualisation in VS and Xcode ######
file(GLOB_RECURSE NON_UNITY_SOURCE src/*.cpp src/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NON_UNITY_SOURCE})
add_custom_target("Editor_Source_Files" SOURCES ${NON_UNITY_SOURCE})