set(ENGINE_DIR ${ROOT_DIR}/Engine)

include(${ROOT_DIR}/CMake/Preprocessor.cmake)
include(${ROOT_DIR}/CMake/IncludeDirs.cmake)
include(${ROOT_DIR}/CMake/LinkDirs.cmake)
include(${ROOT_DIR}/CMake/BuildOptions.cmake)

if (${USE_EDITOR})
    include(${ROOT_DIR}/CMake/PreprocessorEditor.cmake)
    include(${ROOT_DIR}/CMake/IncludeDirsEditor.cmake)
endif()

function(setup_spark_target target use_editor)
    include(${ROOT_DIR}/CMake/Linker.cmake)
    if (${use_editor})
        include(${ROOT_DIR}/CMake/LinkerEditor.cmake)
    endif()
    include(${ROOT_DIR}/CMake/PCH.cmake)
endfunction()