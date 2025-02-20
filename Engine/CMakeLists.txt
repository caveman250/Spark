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
include(${ROOT_DIR}/CMake/ExternalTools.cmake)

###### Editor Includes ######
if (${USE_EDITOR})
    include(${ROOT_DIR}/CMake/PreprocessorEditor.cmake)
    include(${ROOT_DIR}/CMake/IncludeDirsEditor.cmake)
endif()

###### Third Party ######
include(${ROOT_DIR}/CMake/ThirdParty.cmake)

###### Compile Opts (After Third party to avoid spam) ######
include(${ROOT_DIR}/CMake/BuildOptions.cmake)

###### Generate Unity Files #######
execute_process(COMMAND ${PYTHON_EXE} UnityBatcher.py ../Engine/unity.conf ${PLATFORM}
                WORKING_DIRECTORY ${ROOT_DIR}/Spark/Build/)

###### Source ######
file(GLOB SOURCE ${ENGINE_DIR}/unity_generated/*.cpp)

###### Output ######
add_library(Spark ${SOURCE})

###### Linker ######
target_link_libraries(Spark OpenFBX bc7enc)
if (WIN32)
    target_link_libraries(Spark Dwmapi)
endif()

###### Platform Linker ######
if (${PLATFORM} MATCHES Windows)
    include(${ROOT_DIR}/CMake/WindowsLinker.cmake)
elseif (${PLATFORM} MATCHES Linux)
    include(${ROOT_DIR}/CMake/LinuxLinker.cmake)
elseif (${PLATFORM} MATCHES Mac)
    include(${ROOT_DIR}/CMake/MacLinker.cmake)
endif ()

###### PCH ######
target_precompile_headers(Spark PUBLIC src/spark.h)

#target_compile_options(Spark PUBLIC -fsanitize=address)
#target_link_options(Spark PUBLIC -fsanitize=address)
