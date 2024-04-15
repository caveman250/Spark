#pragma once
#include "engine/render/Renderer.h"

namespace se::render::opengl
{
    class OpenGLRenderer : public Renderer
    {
    public:
        OpenGLRenderer();
        void Render(IWindow* window) override;
    };
}
