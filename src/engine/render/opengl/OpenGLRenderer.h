#pragma once
#include "engine/render/Renderer.h"

namespace se::render::opengl
{
    class OpenGLRenderer : public Renderer
    {
        void Render(IWindow* window) override;
    };
}
