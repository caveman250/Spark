#include "engine/render/RenderCommand.h"
#include "spark.h"
#include "GL_fwd.h"
#include "engine/render/Material.h"
#include "engine/render/VertexBuffer.h"


namespace se::render
{
    void RenderCommand::Clear(bool clearColour, bool clearDepth)
    {
        GLbitfield mask = 0;
        if (clearColour)
        {
            mask |= GL_COLOR_BUFFER_BIT;
        }
        if (clearDepth)
        {
            mask |= GL_DEPTH_BUFFER_BIT;
        }

        glClear(mask);
    }

    void RenderCommand::SubmitGeo(const std::shared_ptr<Material>& material, const std::shared_ptr<VertexBuffer>& vertBuffer, int indexCount)
    {
        material->Bind();
        vertBuffer->Bind();

        glDrawArrays(GL_TRIANGLES, 0, indexCount);
        GL_CHECK_ERROR()

        vertBuffer->Unbind();
        GL_CHECK_ERROR()
    }
}
