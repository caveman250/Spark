#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "ImageRenderSystem.h"

#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "engine/ui/util/MeshUtil.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ImageRenderSystem)

    void ImageRenderSystem::OnRender(const std::vector<ecs::Id>& entities,
                                     const components::RectTransformComponent* transformComps,
                                     components::ImageComponent* imageComps,
                                     const components::WidgetComponent* widgetComps,
                                     ui::singleton_components::UIRenderComponent* renderComp)
    {
        PROFILE_SCOPE("ImageRenderSystem::OnRender")

        auto app = Application::Get();
        auto renderer = render::Renderer::Get();
        auto window = app->GetPrimaryWindow();
        auto windowsSize = math::Vec2(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& widget = widgetComps[i];
            if (!widget.renderingEnabled || !widget.parentRenderingEnabled)
            {
                continue;
            }

            const auto& entity = entities[i];
            const auto& transform = transformComps[i];
            auto& image = imageComps[i];

            if (SPARK_VERIFY(image.material))
            {
                if (transform.rect.topLeft != image.lastRect.topLeft)
                {
                    auto floatVec = math::Vec2(transform.rect.topLeft);
                    image.material->SetUniform("pos", asset::shader::ast::AstType::Vec2, 1, &floatVec);
                }

                if (!image.vertBuffer || image.lastRect.size != transform.rect.size)
                {
                    asset::StaticMesh mesh = util::CreateMeshFromRect(transform.rect);
                    image.vertBuffer = render::VertexBuffer::CreateVertexBuffer(mesh);
                    image.vertBuffer->CreatePlatformResource();
                    image.indexBuffer = render::IndexBuffer::CreateIndexBuffer(mesh);
                    image.indexBuffer->CreatePlatformResource();
                }

                image.lastRect = transform.rect;

                image.material->SetUniform("screenSize", asset::shader::ast::AstType::Vec2, 1, &windowsSize);

                auto command = renderer->AllocRenderCommand<render::commands::SubmitUI>(image.material, image.vertBuffer,
                                                             image.indexBuffer, transform.layer);
                renderComp->entityRenderCommands[entity].push_back(command);
            }
        }
    }
}
