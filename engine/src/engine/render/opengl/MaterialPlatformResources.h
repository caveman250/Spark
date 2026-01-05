#pragma once

#include "spark.h"
#include "engine/render/MaterialPlatformResources.h"

#if OPENGL_RENDERER

#include "GL/glew.h"

namespace se::render::opengl
{
    class MaterialPlatformResources : public render::MaterialPlatformResources
    {
    public:
        void Bind() override;
        void CreatePlatformResources(const std::string& vert,
            const std::string& frag,
            const RenderState& rs) override;
        void DestroyPlatformResources() override;

        GLuint GetProgramID() { return m_CompiledProgram; }

    private:
        void ApplyDepthStencil(const RenderState& rs) override;
        void ApplyBlendMode(BlendMode src, BlendMode dest) override;

        GLuint m_VertexShader = GL_INVALID_VALUE;
        GLuint m_FragmentShader = GL_INVALID_VALUE;
        GLuint m_CompiledProgram = GL_INVALID_VALUE;
    };
}

#endif
