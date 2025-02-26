find_package(SDL2 REQUIRED)
target_link_libraries(Spark ${SDL2_LIBRARIES})

find_package(OpenGL REQUIRED)
target_link_libraries(Spark OpenGL::OpenGL glew)