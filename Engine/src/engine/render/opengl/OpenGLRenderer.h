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

        //PIXELFORMATDESCRIPTOR& GetPixelFormatDecriptor() { return m_PFD; };

    private:
        void ApplyDepthCompare(DepthCompare comp) override;
        //PIXELFORMATDESCRIPTOR m_PFD;
    };
}
