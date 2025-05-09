cmake_minimum_required(VERSION 3.28)

set(ROOT_DIR ..)
include(${ROOT_DIR}/CMake/include/Util.cmake)

add_subdirectory_with_folder("Tools" AssetBuilder)
add_subdirectory_with_folder("Tools" BinToText)