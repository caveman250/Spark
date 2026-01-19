#pragma once
#include "engine/render/render_fwd.h"

#if OPENGL_RENDERER
#include "engine/render/Renderer.h"

namespace se::render::opengl
{
    class OpenGLRenderer : public Renderer
    {
    public:
        OpenGLRenderer();
        void Init() override;
        void Render() override;
        RenderAPI GetRenderAPIType() const override { return RenderAPI::OpenGL; }

        bool IsGLEWInitialised() const;

        bool SupportsMultiThreadedRendering() const override;

    private:
        bool m_GlewInit = false;
    };
}
#endif
