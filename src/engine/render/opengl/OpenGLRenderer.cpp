#include "OpenGLRenderer.h"
#include "GL_fwd.h"
#include "engine/logging/Log.h"

#include "engine/render/RenderCommand.h"

namespace se::render::opengl
{
    OpenGLRenderer::OpenGLRenderer()
    {
        glewExperimental = true;
        if (glewInit() != GLEW_OK)
        {
            logging::Log::Fatal("Failed to initialize GLEW");
        }
    }

    void OpenGLRenderer::Render(IWindow* window)
    {
        for (const auto& renderCmd : RenderCommands[window])
        {
            renderCmd.command();
        }
    }
}
