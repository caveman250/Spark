cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)

set(ROOT_DIR ..)
include(${ROOT_DIR}/CMake/include/Spark.cmake)

setup_spark_target(Editor true ${ROOT_DIR}/Editor/unity.conf)

set_property(TARGET Editor PROPERTY EXCLUDE_FROM_DEFAULT_BUILD_DEBUG TRUE)
set_property(TARGET Editor PROPERTY EXCLUDE_FROM_DEFAULT_BUILD_RELEASE TRUE)
set_property(TARGET Editor PROPERTY EXCLUDE_FROM_DEFAULT_BUILD_DISTRIBUTION TRUE)