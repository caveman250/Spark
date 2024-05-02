cmake_minimum_required(VERSION 3.28)
project(Spark)
set(CMAKE_CXX_STANDARD 23)
set(ROOT_DIR ..)
set(ENGINE_DIR .)

include(${ROOT_DIR}/CMake/Preprocessor.cmake)
include(${ROOT_DIR}/CMake/IncludeDirs.cmake)
include(${ROOT_DIR}/CMake/LinkDirs.cmake)

if (${USE_EDITOR})
    include(${ROOT_DIR}/CMake/PreprocessorEditor.cmake)
    include(${ROOT_DIR}/CMake/IncludeDirsEditor.cmake)
endif()

###### Third Party ######
add_subdirectory(third_party/Crunch2-1.3.1)

###### Source ######
file(GLOB PLATFORM_SHARED_CODE src/platform/*.cpp src/platform/*.h)
if (${PLATFORM} MATCHES Windows)
    file(GLOB_RECURSE PLATFORM_CODE src/platform/windows/*.cpp src/platform/windows/*.h)
endif()
file(GLOB_RECURSE ENGINE_CODE src/engine/*.cpp src/engine/*.h)

###### Output ######
add_library(Spark ${PLATFORM_SHARED_CODE} ${PLATFORM_CODE} ${ENGINE_CODE})

###### PCH ######
target_precompile_headers(Spark PUBLIC src/spark.h)

###### Linker ######
target_link_libraries(Spark glew32s opengl32 crn)
if (WIN32)
    target_link_libraries(Spark Dwmapi)
endif()
