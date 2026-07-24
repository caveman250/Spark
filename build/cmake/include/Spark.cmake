set(ENGINE_DIR ${ROOT_DIR}/engine)

include(Preprocessor)
include(Platform)
include(IncludeDirs)
include(LinkDirs)
include(BuildOptions)
include(ExternalTools)
include(editor/PreprocessorEditor)
include(editor/IncludeDirsEditor)
include(editor/LinkerEditor)

function(exclude_files_from_vs files)
    set_property(SOURCE ${files} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")
endfunction()

function(apply_xcode_exclude_attribute target files config)
    get_property(current_value TARGET ${target} PROPERTY XCODE_ATTRIBUTE_EXCLUDED_SOURCE_FILE_NAMES[variant=${config}])
    if(current_value)
        set_property(TARGET ${target} PROPERTY XCODE_ATTRIBUTE_EXCLUDED_SOURCE_FILE_NAMES[variant=${config}] "${current_value}, ${files}")
    else()
        set_property(TARGET ${target} PROPERTY XCODE_ATTRIBUTE_EXCLUDED_SOURCE_FILE_NAMES[variant=${config}] "${files}")
    endif()

endfunction()

function(exclude_files_from_xcode_variant target files config)
    foreach(file in ${files})
        get_filename_component(filename ${file} NAME)
        if (NOT ${filename} STREQUAL "" AND NOT ${filename} STREQUAL "in")
            if (NOT DEFINED excluded_src)
                set(excluded_src "${filename}")
            else()
                set(excluded_src "${excluded_src} ${filename}")
            endif()
        endif()
    endforeach()

    apply_xcode_exclude_attribute(${target} ${excluded_src} ${config})
endfunction()

function(exclude_files_from_xcode target files)
    foreach(file in ${files})
        get_filename_component(filename ${file} NAME)
        if (NOT ${filename} STREQUAL "" AND NOT ${filename} STREQUAL "in")
            if (NOT DEFINED excluded_src)
                set(excluded_src "${filename}")
            else()
                set(excluded_src "${excluded_src} ${filename}")
            endif()
        endif()
    endforeach()

    apply_xcode_exclude_attribute(${target} ${excluded_src} Debug)
    apply_xcode_exclude_attribute(${target} ${excluded_src} DebugEditor)
    apply_xcode_exclude_attribute(${target} ${excluded_src} Release)
    apply_xcode_exclude_attribute(${target} ${excluded_src} ReleaseEditor)
    apply_xcode_exclude_attribute(${target} ${excluded_src} Distribution)
    apply_xcode_exclude_attribute(${target} ${excluded_src} DistributionEditor)
endfunction()

function(setup_source_files target unity_conf_dir is_library)
    message(STATUS "Configuring ${target} source files...")
    #if(TRUE)
        set(unity_build FALSE)
        message(STATUS "-- Unity build disabled.")
        file(GLOB_RECURSE SOURCE src/*.cpp src/*.h generated/*.cpp generated/*.h src/*.mm)
        file(GLOB_RECURSE MODULE_SOURCE src/*.ixx)

        list(FILTER SOURCE EXCLUDE REGEX ".*src/platform/windows/*")
        list(FILTER MODULE_SOURCE EXCLUDE REGEX ".*src/platform/windows/*")
        list(FILTER SOURCE EXCLUDE REGEX ".*src/platform/linux/*")
        list(FILTER MODULE_SOURCE EXCLUDE REGEX ".*src/platform/linux/*")

        if (${PLATFORM} MATCHES Mac)
            set(TARGET_SOURCE ${SOURCE} ${OBJC_SOURCE} ${GENERATED_MODULE_SOURCE})
        else()
            set(TARGET_SOURCE ${SOURCE} ${GENERATED_MODULE_SOURCE})
        endif()
    #else()
    #    set(unity_build TRUE)
    #    message(STATUS "-- Unity build enabled.")
    #    get_filename_component(ABSOLUTE_PATH ${ROOT_DIR}/build/ ABSOLUTE)
    #    execute_process(COMMAND ${PYTHON_EXE} UnityBatcher.py ${unity_conf_dir} ${PLATFORM} WORKING_DIRECTORY ${ABSOLUTE_PATH})
#
    #    file(GLOB SOURCE unity/*.cpp unity/*.h)
    #    file(GLOB OBJC_SOURCE unity/*.mm)
    #    file(GLOB_RECURSE NON_UNITY_SOURCE src/*.cpp src/*.h src/*.mm)
    #    file(GLOB_RECURSE MODULE_SOURCE src/*.ixx)
    #    file(GLOB_RECURSE GENERATED_MODULE_SOURCE src/*.gixx)
    #    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NON_UNITY_SOURCE})
#
    #    if (CMAKE_CONFIGURATION_TYPES)
    #        # add all files for visibility, non unity sources will be excluded below
    #        if (${PLATFORM} MATCHES Mac)
    #            set(TARGET_SOURCE ${SOURCE} ${OBJC_SOURCE} ${NON_UNITY_SOURCE} ${GENERATED_MODULE_SOURCE})
    #        else()
    #            set(TARGET_SOURCE ${SOURCE} ${NON_UNITY_SOURCE} ${GENERATED_MODULE_SOURCE})
    #        endif()
    #    else()
    #        if (${PLATFORM} MATCHES Mac)
    #            set(TARGET_SOURCE ${SOURCE} ${OBJC_SOURCE} ${GENERATED_MODULE_SOURCE})
    #        else()
    #            set(TARGET_SOURCE ${SOURCE} ${GENERATED_MODULE_SOURCE})
    #        endif()
    #    endif()
    #endif()

    set_source_files_properties(
            ${GENERATED_MODULE_SOURCE}
            PROPERTIES LANGUAGE CXX
    )

    if (${is_library})
        message(STATUS "-- Configured library: ${target}\n")
        add_library(${target} STATIC ${TARGET_SOURCE})
    else()
        message(STATUS "-- Configured executable: ${target}\n")
        add_executable(${target} ${TARGET_SOURCE})
    endif()

    target_sources(${target}
            PUBLIC
            FILE_SET modules TYPE CXX_MODULES FILES
            ${MODULE_SOURCE})


    set_target_properties(${target} PROPERTIES EXCLUDE_FROM_ALL TRUE)
    set_target_properties(${target} PROPERTIES ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/unity;${CMAKE_CURRENT_SOURCE_DIR}/src/generated")

    target_compile_options(${target} PRIVATE "-Wno-c2y-extensions")

    # remove all non unity files from the build
    if (${unity_build})
        exclude_files_from_vs("${NON_UNITY_SOURCE}")

        if (CMAKE_GENERATOR MATCHES "Xcode")
            exclude_files_from_xcode(${target} "${NON_UNITY_SOURCE}")

            #make sure we exclude all editor source from non editor builds.
            if(${target} MATCHES Editor)
                if(SOURCE)
                    exclude_files_from_xcode_variant(${target} "${SOURCE}" Debug)
                    exclude_files_from_xcode_variant(${target} "${SOURCE}" Release)
                    exclude_files_from_xcode_variant(${target} "${SOURCE}" Distribution)
                endif()

                if(OBJC_SOURCE)
                    exclude_files_from_xcode_variant(${target} "${OBJC_SOURCE}" Debug)
                    exclude_files_from_xcode_variant(${target} "${OBJC_SOURCE}" Release)
                    exclude_files_from_xcode_variant(${target} "${OBJC_SOURCE}" Distribution)
                endif()
            endif()
        endif()
    endif()

endfunction()

function(setup_target_common target is_library unity_conf_dir)
    include_directories(src)
    include_directories(src/generated)
    setup_source_files(${target} "${unity_conf_dir}" ${is_library})
    setup_compile_options(${target})

    # make sure cmake is reloaded before each build to run header tool and pick up new files.
    # note: disabled in multi config as it breaks vs and xcode.

    if (NOT CMAKE_GENERATOR MATCHES "Xcode")
        add_dependencies(${target} ${REGEN_CMAKE_TARGET})
    endif()

    # xcode gets weird about executables that arent bundles
    if (NOT ${is_library} AND ${PLATFORM} MATCHES Mac)
        set_target_properties(${target} PROPERTIES MACOSX_BUNDLE TRUE)
    endif()

    # Make sure dependant dlls are copied to outp[ut folder for windows builds.
    if (NOT ${is_library} AND ${PLATFORM} MATCHES Windows)
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${target}> $<TARGET_FILE_DIR:${target}>
                COMMAND_EXPAND_LISTS
        )
    endif()

    # Make sure we can actually debug windows builds.
    if (${PLATFORM} MATCHES Windows)
        target_compile_options(${target} PRIVATE /Zi)
    endif()
endfunction()

function(setup_spark_target target is_library unity_conf_dir)
    setup_target_common(${target} ${is_library} "${unity_conf_dir}")

    include(Linker)
    link_editor_libs(${target})
    include(PCH)
endfunction()