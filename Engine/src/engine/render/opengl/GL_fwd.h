#pragma once

#include "engine/render/render_fwd.h"

#if OPENGL_RENDERER

#define GLEW_STATIC
#include "GL/glew.h"
#if SPARK_PLATFORM_WINDOWS
#include "GL/wglew.h"
#endif
#include "GL/gl.h"


#if SPARK_PLATFORM_LINUX
#include "SDL2/SDL_opengl.h"
#endif

#if !SPARK_DIST
#define GL_CHECK_ERROR()\
{\
    bool hasError = false;\
    GLenum error;\
    while ((error = glGetError()) != GL_NO_ERROR)\
    {\
        hasError = true;\
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
    SPARK_ASSERT(!hasError);\
}
#else
#define GL_CHECK_ERROR()
#endif

#endif
