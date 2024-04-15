#include "OpenGLRenderer.h"
#include <gl/GL.h>

#include "engine/render/RenderCommand.h"

namespace se::render::opengl
{
    void OpenGLRenderer::Render(IWindow* window)
    {
        for (const auto& renderCmd : RenderCommands[window])
        {
            renderCmd.command();
        }
    }
}
