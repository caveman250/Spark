#pragma once

#include "spark.h"
#include "GL_fwd.h"
#include "engine/render/IndexBuffer.h"

namespace se::render::opengl
{
    class IndexBuffer : public render::IndexBuffer
    {
    public:
        IndexBuffer(const asset::StaticMesh& mesh);

        void CreatePlatformResource() override;
        void Bind() override;
    private:
        GLuint m_Resource;
    };
}
