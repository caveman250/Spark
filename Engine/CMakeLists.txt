cmake_minimum_required(VERSION 3.28)
project(Spark)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POLICY_DEFAULT_CMP0072 NEW)

set(ROOT_DIR ..)
set(ENGINE_DIR .)

###### Generic Includes ######
include(${ROOT_DIR}/CMake/Preprocessor.cmake)
include(${ROOT_DIR}/CMake/IncludeDirs.cmake)
include(${ROOT_DIR}/CMake/LinkDirs.cmake)

###### Editor Includes ######
if (${USE_EDITOR})
    include(${ROOT_DIR}/CMake/PreprocessorEditor.cmake)
    include(${ROOT_DIR}/CMake/IncludeDirsEditor.cmake)
endif()

###### Third Party ######
add_subdirectory(third_party/Crunch2-1.3.1)
add_subdirectory(third_party/glew-2.1.0/build/cmake)
add_subdirectory(third_party/OpenFBX-365f52c)

if (MSVC)
    target_compile_options(crn PRIVATE /W0)
    target_compile_options(crunch PRIVATE /W0)
    target_compile_options(miniz PRIVATE /W0)
else()
    target_compile_options(crn PRIVATE -w)
    target_compile_options(crunch PRIVATE -w)
    target_compile_options(miniz PRIVATE -w)
endif()

###### Compile Opts (After Third party to avoid spam) ######
include(${ROOT_DIR}/CMake/BuildOptions.cmake)

###### Source ######
file(GLOB PLATFORM_SHARED_CODE src/platform/*.cpp src/platform/*.h)
if (${PLATFORM} MATCHES Windows)
    file(GLOB_RECURSE PLATFORM_CODE src/platform/windows/*.cpp src/platform/windows/*.h)
elseif (${PLATFORM} MATCHES Linux)
    file(GLOB_RECURSE PLATFORM_CODE src/platform/linux/*.cpp src/platform/linux/*.h)
endif()
file(GLOB_RECURSE ENGINE_CODE src/engine/*.cpp src/engine/*.h)

###### Output ######
add_library(Spark ${PLATFORM_SHARED_CODE} ${PLATFORM_CODE} ${ENGINE_CODE})

###### Platform ######
if (${PLATFORM} MATCHES Linux)
    include(${ROOT_DIR}/CMake/Linux.cmake)
endif ()

###### PCH ######
target_precompile_headers(Spark PUBLIC src/spark.h)

###### Linker ######
find_package(OpenGL REQUIRED)
target_link_libraries(Spark glew OpenGL::OpenGL crn OpenFBX)
if (WIN32)
    target_link_libraries(Spark Dwmapi)
endif()
