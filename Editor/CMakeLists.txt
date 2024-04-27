cmake_minimum_required(VERSION 3.28)
project(Editor)
set(CMAKE_CXX_STANDARD 23)

###### Includes ######
include_directories(src)
include_directories(../Engine/src)

###### Linker Dirs ######
link_directories(../Engine/third_party/glew-2.1.0/lib/Release/x64)

###### Source Code ######
file(GLOB_RECURSE SRC src/*.cpp src/*.h)

###### Output ######
add_executable(Editor ${SRC})

###### Linker ######
target_link_libraries(Editor Spark)

###### PCH ######
target_precompile_headers(Editor PUBLIC "../Engine/src/spark.h")