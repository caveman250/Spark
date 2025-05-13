include(${ROOT_DIR}/CMake/include/Util.cmake)
add_subdirectory_with_folder("third_party/glew" third_party/glew-2.1.0/build/cmake)

find_package(OpenMP)
target_link_libraries(bc7enc OpenMP::OpenMP_CXX)