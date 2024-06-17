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
        void ApplyDepthCompare(DepthCompare comp) override;

        bool m_GlewInit = false;
    };
}
