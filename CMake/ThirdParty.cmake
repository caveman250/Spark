set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/bc7enc_rdo)
add_subdirectory(third_party/OpenFBX-365f52c)

if (${PLATFORM} MATCHES Windows)
    include(${ROOT_DIR}/CMake/windows/WindowsThirdParty.cmake)
elseif (${PLATFORM} MATCHES Linux)
    include(${ROOT_DIR}/CMake/linux/LinuxThirdParty.cmake)
elseif (${PLATFORM} MATCHES Mac)
    include(${ROOT_DIR}/CMake/mac/MacThirdParty.cmake)
endif ()

#make third party libraries shut up
if (MSVC)
    target_compile_options(bc7enc PRIVATE /W0)
    target_compile_options(OpenFBX PRIVATE /W0)
else()
    target_compile_options(bc7enc PRIVATE -w)
    target_compile_options(OpenFBX PRIVATE -w)
endif()