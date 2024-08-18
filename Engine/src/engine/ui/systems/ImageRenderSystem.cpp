#include "spark.h"

#include <engine/math/Mat4.h>
#include <engine/ecs/components/MeshComponent.h>
#include "ImageRenderSystem.h"

#include "engine/Application.h"
#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "engine/ui/util/MeshUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ImageRenderSystem)

    void ImageRenderSystem::OnRender(const std::vector<ecs::Id>& entities, const components::RectTransformComponent* transformComps, components::ImageComponent* imageComps)
    {
        auto app = Application::Get();
        auto renderer = render::Renderer::Get();
        auto window = app->GetPrimaryWindow();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& transform = transformComps[i];
            auto& image = imageComps[i];
            if (SPARK_VERIFY(image.material))
            {
                if (!image.vertBuffer || image.lastRect != transform.rect)
                {
                    asset::StaticMesh mesh = util::MeshUtil::CreateMeshFromRect(transform.rect);
                    image.vertBuffer = render::VertexBuffer::CreateVertexBuffer(mesh);
                    image.vertBuffer->CreatePlatformResource();
                    image.indexBuffer = render::IndexBuffer::CreateIndexBuffer(mesh);
                    image.indexBuffer->CreatePlatformResource();
                    image.lastRect = transform.rect;
                }

                renderer->Submit<render::commands::SubmitGeo>(window, image.material, image.vertBuffer, image.indexBuffer);
            }
        }
    }
}
