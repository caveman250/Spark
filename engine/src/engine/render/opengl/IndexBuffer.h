#pragma once

#include "engine/render/render_fwd.h"

#if OPENGL_RENDERER

#include "spark.h"
#include "GL_fwd.h"
#include "engine/render/IndexBuffer.h"

namespace se::render::opengl
{
    class IndexBuffer : public render::IndexBuffer
    {
    public:
        IndexBuffer(const asset::StaticMesh& mesh);
        ~IndexBuffer();

        void CreatePlatformResource() override;
        void Bind() override;
    private:
        void Cleanup();
        void Cleanup(GLuint resource);

        GLuint m_Resource = GL_INVALID_VALUE;
    };
}

#endif
