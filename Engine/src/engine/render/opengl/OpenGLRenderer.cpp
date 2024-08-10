#include "OpenGLRenderer.h"
#include "GL_fwd.h"

#include "engine/render/RenderCommand.h"
#include "platform/IWindow.h"

namespace se::render::opengl
{
    OpenGLRenderer::OpenGLRenderer()
    {

    }

    void OpenGLRenderer::Init()
    {
        auto window = IWindow::CreatePlatformWindow(500, 500);

        glewExperimental = true;
        if (glewInit() != GLEW_OK)
        {
            debug::Log::Fatal("Failed to initialize GLEW");
        }

        m_GlewInit = true;

        window->OnClose();

        const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
        const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model

        debug::Log::Info("GPU Vendor: {}", (char*)vendor);
        debug::Log::Info("GPU model: {}", (char*)renderer);
    }

    void OpenGLRenderer::Render(IWindow* window)
    {
        Renderer::Render(window);
        glFlush();
    }

    void OpenGLRenderer::ApplyDepthCompare(DepthCompare comp)
    {
        if (comp == DepthCompare::None)
        {
            glDisable(GL_DEPTH_TEST);
            return;
        }
        else
        {
            glEnable(GL_DEPTH_TEST);
        }

        GL_CHECK_ERROR()

        switch (comp)
        {
        case DepthCompare::Less:
            glDepthFunc(GL_LESS);
            break;
        case DepthCompare::LessEqual:
            glDepthFunc(GL_LEQUAL);
            break;
        case DepthCompare::Equal:
            glDepthFunc(GL_EQUAL);
            break;
        case DepthCompare::Greater:
            glDepthFunc(GL_GREATER);
            break;
        case DepthCompare::GreaterEqual:
            glDepthFunc(GL_GEQUAL);
            break;
        default:
            SPARK_ASSERT(false);
            break;
        }

        GL_CHECK_ERROR()
    }

    bool OpenGLRenderer::IsGLEWInitialised() const
    {
        return m_GlewInit;
    }
}
