#include "OpenGLRenderer.h"
#if OPENGL_RENDERER
#include "GL_fwd.h"

#include "engine/render/RenderCommand.h"
#include "platform/IWindow.h"

namespace se::render::opengl
{
    GLuint BlendModeToGLBlendMode(BlendMode::Type blendMode)
    {
        switch (blendMode)
        {
            case BlendMode::Zero:
                return GL_ZERO;
            case BlendMode::One:
                return GL_ONE;
            case BlendMode::SrcColor:
                return GL_SRC_COLOR;
            case BlendMode::OneMinusSrcColor:
                return GL_ONE_MINUS_SRC_COLOR;
            case BlendMode::DstColor:
                return GL_DST_COLOR;
            case BlendMode::OneMinusDstColor:
                return GL_ONE_MINUS_DST_COLOR;
            case BlendMode::SrcAlpha:
                return GL_SRC_ALPHA;
            case BlendMode::OneMinusSrcAlpha:
                return GL_ONE_MINUS_SRC_ALPHA;
            case BlendMode::DstAlpha:
                return GL_DST_ALPHA;
            case BlendMode::OneMinusDstAlpha:
                return GL_ONE_MINUS_DST_ALPHA;
            default:
                SPARK_ASSERT(false);
            return GL_ZERO;
        }
    }

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
        GL_CHECK_ERROR()
    }

    void OpenGLRenderer::ApplyDepthStencil(DepthCompare::Type comp, StencilFunc::Type src, uint32_t writeMask, uint32_t readMask)
    {
        if (comp == DepthCompare::None)
        {
            glDisable(GL_DEPTH_TEST);
            GL_CHECK_ERROR()
        }
        else
        {
            glEnable(GL_DEPTH_TEST);
            GL_CHECK_ERROR()
        }

        if (comp != DepthCompare::None)
        {
            switch (comp)
            {
                case DepthCompare::Less:
                    glDepthFunc(GL_LESS);
                    GL_CHECK_ERROR()
                    break;
                case DepthCompare::LessEqual:
                    glDepthFunc(GL_LEQUAL);
                    GL_CHECK_ERROR()
                    break;
                case DepthCompare::Equal:
                    glDepthFunc(GL_EQUAL);
                    GL_CHECK_ERROR()
                    break;
                case DepthCompare::Greater:
                    glDepthFunc(GL_GREATER);
                    GL_CHECK_ERROR()
                    break;
                case DepthCompare::GreaterEqual:
                    glDepthFunc(GL_GEQUAL);
                    GL_CHECK_ERROR()
                    break;
                default:
                    SPARK_ASSERT(false);
                    break;
            }

            GL_CHECK_ERROR()
        }

        if (!writeMask && func == StencilFunc::None)
        {
            glDisable(GL_STENCIL_TEST);
            GL_CHECK_ERROR()
            return;
        }

        glEnable(GL_STENCIL_TEST);
        GL_CHECK_ERROR()
        glStencilMask(writeMask);
        GL_CHECK_ERROR()
        if (func != StencilFunc::None)
        {
            switch (func)
            {
                case StencilFunc::Less:
                    glStencilFunc(GL_LESS, 1, readMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::LessEqual:
                    glStencilFunc(GL_LEQUAL, 1, readMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::Equal:
                    glStencilFunc(GL_EQUAL, 1, readMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::Greater:
                    glStencilFunc(GL_GREATER, 1, readMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::GreaterEqual:
                    glStencilFunc(GL_GEQUAL, 1, readMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::None:
                    glStencilFunc(GL_NEVER, 0, 0);
                    GL_CHECK_ERROR()
                    break;
            }
        }
    }

    void OpenGLRenderer::ApplyBlendMode(BlendMode::Type src, BlendMode::Type dst)
    {
        if (src == BlendMode::None || dst == BlendMode::None)
        {
            glDisable(GL_BLEND);
            GL_CHECK_ERROR()
            return;
        }
        else
        {
            glEnable(GL_BLEND);
            GL_CHECK_ERROR()
        }

        glBlendFunc(BlendModeToGLBlendMode(src), BlendModeToGLBlendMode(dst));
        GL_CHECK_ERROR()
    }

    bool OpenGLRenderer::IsGLEWInitialised() const
    {
        return m_GlewInit;
    }
}
#endif