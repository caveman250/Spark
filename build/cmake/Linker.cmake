if(${PLATFORM} MATCHES Linux OR NOT ${target} MATCHES Editor) # prevent duplicate library warning. for some reason not an issue on linux.
    target_link_libraries(${target} Spark)
    target_link_libraries(${target} OpenFBX bc7enc)
endif()