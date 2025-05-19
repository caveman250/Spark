#pragma once
#include "engine/render/render_fwd.h"

#if OPENGL_RENDERER

#include "engine/asset/texture/Texture.h"
#include "engine/render/Material.h"
#include "GL_fwd.h"

namespace se::render::opengl
{
    class Material : public render::Material
    {
    public:
        Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                 const std::vector<std::shared_ptr<asset::Shader>>& fragShaders);
        void Bind(const render::VertexBuffer&) override;
        void CreatePlatformResources(const render::VertexBuffer& vb) override;
        void DestroyPlatformResources() override;

        GLuint GetVertexShader() { return m_VertexShader; }
        GLuint GetFragmentShader() { return m_FragmentShader; }
    private:
        void ApplyDepthStencil(DepthCompare comp, StencilFunc src, uint32_t writeMask, uint32_t readMask) override;
        void ApplyBlendMode(BlendMode src, BlendMode dest) override;

        GLuint m_VertexShader = GL_INVALID_VALUE;
        GLuint  m_FragmentShader = GL_INVALID_VALUE;
    };
}
#endif