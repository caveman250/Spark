cmake_minimum_required(VERSION 3.28)
project(Spark)
set(CMAKE_CXX_STANDARD 23)

###### Include ######
include_directories(src)
include_directories(third_party/glew-2.1.0/include)

###### Source ######
file(GLOB PLATFORM_SHARED_CODE src/platform/*.cpp src/platform/*.h)
if (${PLATFORM} MATCHES Windows)
    file(GLOB_RECURSE PLATFORM_CODE src/platform/windows/*.cpp src/platform/windows/*.h)
endif()
file(GLOB_RECURSE ENGINE_CODE src/engine/*.cpp src/engine/*.h)

###### Output ######
add_library(Spark ${PLATFORM_SHARED_CODE} ${PLATFORM_CODE} ${ENGINE_CODE})

###### PCH ######
target_precompile_headers(Spark PUBLIC "src/spark.h")

###### Linker ######
target_link_libraries(Spark glew32s opengl32)
if (WIN32)
    target_link_libraries(Spark Dwmapi)
endif()
