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

    bool OpenGLRenderer::IsGLEWInitialised() const
    {
        return m_GlewInit;
    }
}
#endif