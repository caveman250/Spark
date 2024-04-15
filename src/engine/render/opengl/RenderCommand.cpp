#include "engine/render/RenderCommand.h"
#include "spark.h"
#include "GL_fwd.h"
#include "engine/render/Material.h"
#include "engine/render/VertexBuffer.h"

namespace se::render
{
    void RenderCommand::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderCommand::SubmitGeo(const std::shared_ptr<Material>& material, const std::shared_ptr<VertexBuffer>& vertBuffer, int indexCount)
    {
        material->Bind();
        vertBuffer->Bind();

        glDrawArrays(GL_TRIANGLES, 0, 3);

        vertBuffer->Unbind();
    }
}
