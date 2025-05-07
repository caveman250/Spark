cmake_minimum_required(VERSION 3.28)

set(ROOT_DIR ..)
include(${ROOT_DIR}/CMake/include/Util.cmake)

add_subdirectory_with_folder("tools" AssetBuilder)
add_subdirectory_with_folder("tools" BinToText)