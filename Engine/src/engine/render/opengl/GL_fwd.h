#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#if SPARK_PLATFORM_LINUX
#include <SDL2/SDL_opengl.h>
#endif

#if SPARK_PLATFORM_WINDOWS
#include <GL/wglew.h>
#endif

#if !SPARK_DIST
#define GL_CHECK_ERROR()\
{\
    GLenum error = glGetError();\
    if (error != GL_NONE)\
    {\
        debug::Log::Error((char*)glewGetErrorString(error));\
    }\
}
#else
#define GL_CHECK_ERROR()
#endif
