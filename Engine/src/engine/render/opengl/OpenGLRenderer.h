#pragma once
#include "engine/render/Renderer.h"

namespace se::render::opengl
{
    class OpenGLRenderer : public Renderer
    {
    public:
        OpenGLRenderer();
        void Init() override;
        void Render(IWindow* window) override;

        bool IsGLEWInitialised() const;

    private:
        void ApplyDepthCompare(DepthCompare::Type comp) override;
        void ApplyBlendMode(BlendMode::Type src, BlendMode::Type dest) override;
        void ApplyStencil(StencilFunc::Type src, uint32_t writeMask, uint32_t readMask) override;

        bool m_GlewInit = false;
    };
}
