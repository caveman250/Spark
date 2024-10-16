if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if (WIN32)
# TODO
elseif (UNIX)
    if(CMAKE_BUILD_TYPE MATCHES Release)
        add_compile_options(-Wno-unused-parameter)
    endif()
endif()