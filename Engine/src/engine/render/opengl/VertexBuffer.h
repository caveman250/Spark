#pragma once

#include "engine/render/VertexBuffer.h"
#include "GL_fwd.h"

namespace se::render::opengl
{
    class VertexBuffer : public render::VertexBuffer
    {
    public:
        VertexBuffer(const asset::StaticMesh& mesh);

        void CreatePlatformResource() override;
        void Bind() override;
        void Unbind() override;
    private:
        std::unordered_map<VertexStreamType, GLuint> m_GlResources;
    };
}
