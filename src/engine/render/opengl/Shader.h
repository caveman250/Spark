#pragma once

#include "engine/render/Shader.h"
#include "GL_fwd.h"

namespace se::render::opengl
{
    class Shader : public render::Shader
    {
        GLuint programId;
    };
}
