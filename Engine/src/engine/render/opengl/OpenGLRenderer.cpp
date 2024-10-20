#include "OpenGLRenderer.h"
#include "GL_fwd.h"

#include "engine/render/RenderCommand.h"
#include "platform/IWindow.h"

namespace se::render::opengl
{
    OpenGLRenderer::OpenGLRenderer()
    {

    }

    void DebugCallbackARB(GLenum,
                      GLenum,
                      GLuint,
                      GLenum,
                      GLsizei,
                      const GLchar* message,
                      const GLvoid*)
    {
        debug::Log::Error(message);
    }

    void OpenGLRenderer::Init()
    {
        auto window = IWindow::CreatePlatformWindow(500, 500);

        glewExperimental = true;
        if (glewInit() != GLEW_OK)
        {
            debug::Log::Fatal("Failed to initialize GLEW");
        }

#if SPARK_DEBUG
        glEnable              ( GL_DEBUG_OUTPUT );
        glDebugMessageCallback( &DebugCallbackARB, 0 );
#endif

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

    void OpenGLRenderer::ApplyDepthCompare(DepthCompare::Type comp)
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

    void OpenGLRenderer::ApplyBlendMode(BlendMode::Type src, BlendMode::Type dst)
    {
        if (src == BlendMode::None || dst == BlendMode::None)
        {
            glDisable(GL_BLEND);
            return;
        }
        else
        {
            glEnable(GL_BLEND);
        }

        GL_CHECK_ERROR()
        glBlendFunc(BlendModeToGLBlendMode(src), BlendModeToGLBlendMode(dst));

        GL_CHECK_ERROR()
    }

    void OpenGLRenderer::ApplyStencil(StencilFunc::Type func, uint32_t writeMask, uint32_t readMask)
    {
        if (!writeMask && func == StencilFunc::None)
        {
            glDisable(GL_STENCIL_TEST);
            return;
        }

        glEnable(GL_STENCIL_TEST);
        glStencilMask(writeMask);
        if (func != StencilFunc::None)
        {
            switch (func)
            {
                case StencilFunc::Less:
                    glStencilFunc(GL_LESS, 1, readMask);
                    break;
                case StencilFunc::LessEqual:
                    glStencilFunc(GL_LEQUAL, 1, readMask);
                    break;
                case StencilFunc::Equal:
                    glStencilFunc(GL_EQUAL, 1, readMask);
                    break;
                case StencilFunc::Greater:
                    glStencilFunc(GL_GREATER, 1, readMask);
                    break;
                case StencilFunc::GreaterEqual:
                    glStencilFunc(GL_GEQUAL, 1, readMask);
                    break;
                case StencilFunc::None:
                    glStencilFunc(GL_NEVER, 0, 0);
                    break;
            }
        }
    }

    bool OpenGLRenderer::IsGLEWInitialised() const
    {
        return m_GlewInit;
    }
}
