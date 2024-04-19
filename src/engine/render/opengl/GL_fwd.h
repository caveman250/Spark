#pragma once

#define GLEW_STATIC
#include <Gl/glew.h>
#include <gl/GL.h>
#include <GL/wglew.h> // probably only want this on windows?

#if !SPARK_DIST
#define GL_CHECK_ERROR()\
{\
    GLenum error = glGetError();\
    if (error != GL_NONE)\
    {\
        logging::Log::Fatal((char*)glewGetErrorString(error));\
    }\
}
#else
#define GL_CHECK_ERROR()
#endif
