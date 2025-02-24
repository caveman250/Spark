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
        bool m_GlewInit = false;
    };
}
#endif
