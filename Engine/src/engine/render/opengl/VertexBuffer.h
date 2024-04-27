#pragma once

#include "engine/render/VertexBuffer.h"
#include "GL_fwd.h"

namespace se::render::opengl
{
    class VertexBuffer : public render::VertexBuffer
    {
    public:
        void CreatePlatformResource() override;
        void Bind() override;
        void Unbind() override;

        VertexBuffer(const std::vector<VertexStream>& streams);

    private:
        std::unordered_map<VertexStreamType, GLuint> m_GlResources;
    };
}
