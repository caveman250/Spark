cmake_minimum_required(VERSION 3.28)
project(Spark)
set(CMAKE_CXX_STANDARD 23)

set(ROOT_DIR ../..)
include(${ROOT_DIR}/CMake/include/Spark.cmake)

setup_spark_target(AssetBuilder false OFF "")