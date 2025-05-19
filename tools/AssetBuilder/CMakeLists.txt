cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)

set(ROOT_DIR ../..)
include(include/Spark)

setup_spark_target(AssetBuilder false "")