cmake_minimum_required(VERSION 3.28)
project(Spark)
set(CMAKE_CXX_STANDARD 23)

set(ROOT_DIR ..)
include(${ROOT_DIR}/CMake/Spark.cmake)

###### Includes ######
include_directories(src)

###### Generate Unity Files #######
execute_process(COMMAND python UnityBatcher.py ../Editor/unity.conf ${PLATFORM}
        WORKING_DIRECTORY ${ROOT_DIR}/Spark/Build/)

###### Source ######
file(GLOB SOURCE ./unity_generated/*.cpp)

###### Output ######
add_library(Editor ${SOURCE})

setup_spark_target(Editor OFF)