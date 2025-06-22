#pragma once

#include "spark.h"
#include "engine/render/FrameBuffer.h"
#include "GL_fwd.h"

#if OPENGL_RENDERER

namespace se::asset
{
    class Texture;
}

namespace se::render::opengl
{
    class FrameBuffer : public render::FrameBuffer
    {
    public:
        FrameBuffer(const math::IntVec2&);
    private:
        void PreRender() override {}
        void Bind() override;
        void UnBind() override;
        void Commit() override;

        GLuint m_PlatformResource = GL_INVALID_VALUE;
    };
}

#endif
