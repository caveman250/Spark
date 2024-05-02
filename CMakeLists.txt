cmake_minimum_required(VERSION 3.28)
project(Spark)

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE) #make third party libs using old cmake versions shut up.

option(USE_EDITOR "" OFF)

add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(Tools)
add_subdirectory(TestProject)