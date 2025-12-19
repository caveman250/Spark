cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)

set(ROOT_DIR ..)
include(include/Spark)

setup_spark_target(Editor true ${CMAKE_CURRENT_SOURCE_DIR}/unity.conf)

set_property(TARGET Editor PROPERTY EXCLUDE_FROM_DEFAULT_BUILD_DEBUG TRUE)
set_property(TARGET Editor PROPERTY EXCLUDE_FROM_DEFAULT_BUILD_RELEASE TRUE)
set_property(TARGET Editor PROPERTY EXCLUDE_FROM_DEFAULT_BUILD_DISTRIBUTION TRUE)

#target_compile_options(Editor PUBLIC -fsanitize=address)
#target_link_options(Editor PUBLIC -fsanitize=address)
