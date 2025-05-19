if (${USE_EDITOR})
    add_definitions(-DSPARK_EDITOR=1)
endif()

if (CMAKE_CONFIGURATION_TYPES)
    add_compile_definitions($<$<CONFIG:DebugEditor>:SPARK_EDITOR=1>)
    add_compile_definitions($<$<CONFIG:ReleaseEditor>:SPARK_EDITOR=1>)
    add_compile_definitions($<$<CONFIG:DistributionEditor>:SPARK_EDITOR=1>)
endif()