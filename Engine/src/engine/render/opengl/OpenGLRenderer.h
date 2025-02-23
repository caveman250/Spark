#pragma once
#include "engine/render/render_fwd.h"

#if OPENGL_RENDERER
#include "engine/render/Renderer.h"

namespace se::render::opengl
{
    GLuint BlendModeToGLBlendMode(BlendMode::Type blendMode);

    class OpenGLRenderer : public Renderer
    {
    public:
        OpenGLRenderer();
        void Init() override;
        void Render(IWindow* window) override;
        RenderAPI::Type GetRenderAPIType() const override { return RenderAPI::OpenGL; }

        bool IsGLEWInitialised() const;

    private:
        void ApplyDepthStencil(DepthCompare::Type comp, StencilFunc::Type src, uint32_t writeMask, uint32_t readMask) override;
        void ApplyBlendMode(BlendMode::Type src, BlendMode::Type dest) override;

        bool m_GlewInit = false;
    };
}
#endif
