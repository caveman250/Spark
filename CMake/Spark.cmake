set(ENGINE_DIR ${ROOT_DIR}/Engine)

include(${ROOT_DIR}/CMake/Preprocessor.cmake)

include(${ROOT_DIR}/CMake/IncludeDirs.cmake)
include(${ROOT_DIR}/CMake/LinkDirs.cmake)

function(setup_spark_target target)
    include(${ROOT_DIR}/CMake/Linker.cmake)
    include(${ROOT_DIR}/CMake/PCH.cmake)
endfunction()