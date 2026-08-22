#target_precompile_headers(${target} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:spark.h>)

#if(MSVC)
#    target_compile_options(${target} PUBLIC /FI"spark.h")
#else()
#    target_compile_options(${target} PUBLIC -include spark.h)
#endif()