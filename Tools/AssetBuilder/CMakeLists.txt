cmake_minimum_required(VERSION 3.28)
project(Spark)
set(CMAKE_CXX_STANDARD 23)

set(ROOT_DIR ../..)
include(${ROOT_DIR}/CMake/Spark.cmake)

###### Includes ######
include_directories(src)

###### Source Code ######
file(GLOB_RECURSE SRC src/*.cpp src/*.h)

###### Output ######
add_executable(AssetBuilder ${SRC})
setup_spark_target(AssetBuilder OFF)