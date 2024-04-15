#pragma once
#include <Gl/glew.h>

#include "engine/render/VertexBuffer.h"

namespace se::render::opengl
{
    class VertexBuffer : public render::VertexBuffer
    {
    public:
        void CreatePlatformResource() override;
        void Bind() override;
        void Unbind() override;

        VertexBuffer(const std::vector<math::Vec3>& vertices);

    private:
        GLuint m_GlResource;
    };
}
