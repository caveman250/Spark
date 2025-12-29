#include "spark.h"

#include "engine/ecs/components/MeshComponent.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "ImageRenderSystem.h"

#include "engine/Application.h"
#include <easy/profiler.h>
#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/MeshUtil.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration ImageRenderSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Image Render System")
                    .WithComponent<const components::RectTransformComponent>()
                    .WithComponent<components::ImageComponent>()
                    .WithComponent<const components::WidgetComponent>()
                    .WithSingletonComponent<singleton_components::UIRenderComponent>();
    }

    void ImageRenderSystem::OnRender(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("ImageRenderSystem::OnRender");

        auto app = Application::Get();
        auto renderer = render::Renderer::Get<render::Renderer>();
        auto window = app->GetWindow();

        const auto& entities = updateData.GetEntities();
        const auto* widgetComps = updateData.GetComponentArray<const components::WidgetComponent>();
        const auto* transformComps = updateData.GetComponentArray<const components::RectTransformComponent>();
        auto* imageComps = updateData.GetComponentArray<components::ImageComponent>();
        auto* renderComp = updateData.GetSingletonComponent<singleton_components::UIRenderComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& widget = widgetComps[i];
            if (widget.visibility != Visibility::Visible || widget.parentVisibility != Visibility::Visible)
            {
                continue;
            }

            const auto& entity = entities[i];
            const auto& transform = transformComps[i];
            auto& image = imageComps[i];

            if (SPARK_VERIFY(image.materialInstance))
            {
                if (transform.rect.topLeft != image.lastRect.topLeft)
                {
                    auto floatVec = math::Vec2(transform.rect.topLeft);
                    image.materialInstance->SetUniform("pos", asset::shader::ast::AstType::Vec2, 1, &floatVec);
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

                math::Vec2 windowSize = entity.HasFlag(ecs::IdFlags::Editor) ?
                     math::IntVec2(window->GetWidth(), window->GetHeight()) :
                     Application::Get()->GetGameViewportSize();

                image.materialInstance->SetUniform("screenSize", asset::shader::ast::AstType::Vec2, 1, &windowSize);

                auto command = renderer->AllocRenderCommand<render::commands::SubmitUI>(image.materialInstance, image.vertBuffer,
                                                             image.indexBuffer);
                renderComp->entityRenderCommands[entity].push_back(UIRenderCommand(command, UILayerKey(transform.layer, entity.HasFlag(ecs::IdFlags::Editor))));
            }
        }
    }
}
