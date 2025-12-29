include(include/Util)

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_VERSION_MINIMUM "3.5" CACHE STRING "" FORCE)

set(MESSAGE_QUIET ON)

add_subdirectory_with_folder("Third Party" third_party/bc7enc_rdo)
add_subdirectory_with_folder("Third Party" third_party/OpenFBX-365f52c)
add_subdirectory_with_folder("Third Party" third_party/easy_profiler-2.1.0)

if (${PLATFORM} MATCHES Windows)
    include(windows/WindowsThirdParty)
elseif (${PLATFORM} MATCHES Linux)
    include(linux/LinuxThirdParty)
elseif (${PLATFORM} MATCHES Mac)
    include(mac/MacThirdParty)
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