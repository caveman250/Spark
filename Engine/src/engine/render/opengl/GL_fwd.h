#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#if SPARK_PLATFORM_WINDOWS
#include <GL/wglew.h>
#endif
#include <GL/gl.h>

#if SPARK_PLATFORM_LINUX
#include <SDL2/SDL_opengl.h>
#endif

#if !SPARK_DIST
#define GL_CHECK_ERROR()\
{\
    GLenum error = glGetError();\
    if (error != GL_NONE)\
    {\
        switch(error)\
        {\
        case GL_INVALID_ENUM: debug::Log::Error("GL_INVALID_ENUM"); break;\
        case GL_INVALID_VALUE: debug::Log::Error("GL_INVALID_VALUE"); break;\
        case GL_INVALID_OPERATION: debug::Log::Error("GL_INVALID_OPERATION"); break;\
        case GL_STACK_OVERFLOW: debug::Log::Error("GL_STACK_OVERFLOW"); break;\
        case GL_STACK_UNDERFLOW: debug::Log::Error("GL_STACK_UNDERFLOW"); break;\
        case GL_OUT_OF_MEMORY: debug::Log::Error("GL_OUT_OF_MEMORY"); break;\
        case GL_INVALID_FRAMEBUFFER_OPERATION: debug::Log::Error("GL_INVALID_FRAMEBUFFER_OPERATION"); break;\
        case GL_CONTEXT_LOST: debug::Log::Error("GL_CONTEXT_LOST"); break;\
        default: debug::Log::Error("Unknown GL Error"); break;\
        }\
    }\
}
#else
#define GL_CHECK_ERROR()
#endif
