#pragma once
#include "engine/render/render_fwd.h"

#if OPENGL_RENDERER

#include "engine/render/VertexBuffer.h"
#include "GL_fwd.h"

namespace se::render::opengl
{
    class VertexBuffer : public render::VertexBuffer
    {
    public:
        VertexBuffer(const asset::StaticMesh& mesh);
        ~VertexBuffer();

        void CreatePlatformResource() override;
        void Bind() override;
        void Unbind() override;
    private:
        void Cleanup();

        std::unordered_map<VertexStreamType, GLuint> m_GlResources;
        GLuint m_VertexArrayID = GL_INVALID_VALUE;
    };
}
#endif
