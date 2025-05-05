#pragma once

#include "spark.h"
#include "engine/render/FrameBuffer.h"
#include "GL_fwd.h"

namespace se::asset
{
    class Texture;
}

namespace se::render::opengl
{
    class FrameBuffer : public render::FrameBuffer
    {
    public:
        FrameBuffer();
    private:
        void Bind() override;
        void Commit() override;

        GLuint m_PlatformResource = GL_INVALID_VALUE;
    };
}
