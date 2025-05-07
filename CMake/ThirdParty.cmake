include(${ROOT_DIR}/CMake/include/Util.cmake)

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

set(MESSAGE_QUIET ON)

add_subdirectory_with_folder("third_party" third_party/bc7enc_rdo)
add_subdirectory_with_folder("third_party" third_party/OpenFBX-365f52c)

if (${PLATFORM} MATCHES Windows)
    include(${ROOT_DIR}/CMake/windows/WindowsThirdParty.cmake)
elseif (${PLATFORM} MATCHES Linux)
    include(${ROOT_DIR}/CMake/linux/LinuxThirdParty.cmake)
elseif (${PLATFORM} MATCHES Mac)
    include(${ROOT_DIR}/CMake/mac/MacThirdParty.cmake)
endif ()

set(MESSAGE_QUIET OFF)

#make third party libraries shut up
if (MSVC)
    target_compile_options(bc7enc PRIVATE /W0)
    target_compile_options(OpenFBX PRIVATE /W0)
else()
    target_compile_options(bc7enc PRIVATE -w)
    target_compile_options(OpenFBX PRIVATE -w)
endif()

# prevent third party libs generating an ALL_BUILD target
set_target_properties(bc7enc PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(OpenFBX PROPERTIES EXCLUDE_FROM_ALL TRUE)

#make them wait for cmake regen so we dont get permission denied errors under ninja
if(NOT CMAKE_CONFIGURATION_TYPES)
    add_dependencies(bc7enc ${REGEN_CMAKE_TARGET})
    add_dependencies(OpenFBX ${REGEN_CMAKE_TARGET})
endif()