set(ENGINE_DIR ${ROOT_DIR}/Engine)

include(${ROOT_DIR}/CMake/Platform.cmake)
include(${ROOT_DIR}/CMake/Preprocessor.cmake)
include(${ROOT_DIR}/CMake/IncludeDirs.cmake)
include(${ROOT_DIR}/CMake/LinkDirs.cmake)
include(${ROOT_DIR}/CMake/BuildOptions.cmake)
include(${ROOT_DIR}/CMake/ExternalTools.cmake)

if (${USE_EDITOR})
    include(${ROOT_DIR}/CMake/editor/PreprocessorEditor.cmake)
    include(${ROOT_DIR}/CMake/editor/IncludeDirsEditor.cmake)
endif()

function(setup_source_files target unity_conf_dir is_library)
    message("Configuring ${target} source files")
    if("${unity_conf_dir}" STREQUAL "")
        message("${target} unity build: false")
        file(GLOB_RECURSE SOURCE src/*.cpp src/*.h)
        file(GLOB_RECURSE OBJC_SOURCE src/*.mm)
    else()
        get_filename_component(ABSOLUTE_PATH ${ROOT_DIR}/Build/ ABSOLUTE)
        execute_process(COMMAND ${PYTHON_EXE} UnityBatcher.py ${unity_conf_dir} ${PLATFORM} WORKING_DIRECTORY ${ABSOLUTE_PATH})

        file(GLOB SOURCE ./unity_generated/*.cpp)
        file(GLOB OBJC_SOURCE ./unity_generated/*.mm)

        ###### Add non Unity files for visualisation in VS and Xcode ######
        file(GLOB_RECURSE NON_UNITY_SOURCE src/*.cpp src/*.h src/*.mm)
        source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NON_UNITY_SOURCE})
        add_custom_target("${target}_Source_Files" SOURCES ${NON_UNITY_SOURCE})
    endif()



    if (${is_library})
        message("Adding library: ${target}")
        add_library(${target} ${SOURCE} ${OBJC_SOURCE})
    else()
        message("Adding executable: ${target}")
        add_executable(${target} ${SOURCE} ${OBJC_SOURCE})
    endif()
endfunction()

function(setup_spark_target target is_library use_editor unity_conf_dir)
    include_directories(src)
    setup_source_files(${target} "${unity_conf_dir}" ${is_library})

    include(${ROOT_DIR}/CMake/Linker.cmake)
    if (${use_editor})
        include(${ROOT_DIR}/CMake/editor/LinkerEditor.cmake)
    endif()
    include(${ROOT_DIR}/CMake/PCH.cmake)

    #enable parallel std::for_each
    if (${PLATFORM} MATCHES Linux)
        find_package(TBB REQUIRED COMPONENTS tbb)
        target_link_libraries(TestProject tbb)
    endif()
endfunction()