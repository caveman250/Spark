#include "OpenGLRenderer.h"
#if OPENGL_RENDERER
#include "GL_fwd.h"
#include "engine/Application.h"
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

        const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
        const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model

        debug::Log::Info("GPU Vendor: {}", (char*)vendor);
        debug::Log::Info("GPU model: {}", (char*)renderer);

        window->OnClose();
        window->Cleanup();
        delete window;
    }

    void OpenGLRenderer::Render()
    {
        auto window = Application::Get()->GetWindow();
        glViewport(0, 0, window->GetWidth(), window->GetHeight());

        Renderer::Render();
        glFlush();
        GL_CHECK_ERROR()
    }

    bool OpenGLRenderer::IsGLEWInitialised() const
    {
        return m_GlewInit;
    }

    bool OpenGLRenderer::SupportsMultiThreadedRendering() const
    {
        return false;
    }
}
#endif