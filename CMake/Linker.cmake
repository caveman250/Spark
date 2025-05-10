if(NOT ${target} MATCHES Editor) # prevent duplicate library warning.
    target_link_libraries(${target} Spark)
endif()