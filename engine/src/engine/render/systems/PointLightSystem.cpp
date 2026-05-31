#include "PointLightSystem.h"

#include "engine/render/Renderer.h"
#include "engine/render/components/PointLightComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/geo/util/MeshUtil.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/IndexBuffer.h"
#include "engine/render/MaterialInstance.h"
#include "engine/camera/ActiveCameraComponent.h"

namespace se::render::systems
{
    ecs::SystemDeclaration PointLightSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("PointLightSystem")
                .WithComponent<components::PointLightComponent>()
                .WithComponent<const ecs::components::TransformComponent>()
                .WithSingletonComponent<const camera::ActiveCameraComponent>();
    }

    void PointLightSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto renderer = Renderer::Get<Renderer>();

#if SPARK_EDITOR
        auto* editor = Application::Get()->GetEditor();
        if (editor->GetMode() == editor::EditorMode::Prefab)
        {
            renderer->AddPointLight( PointLight{ math::Vec3(-5, 5, 5), math::Vec4(1.f, 1.f, 1.f, 1.f) });
            renderer->AddPointLight( PointLight{ math::Vec3(5, 5, 5), math::Vec4(1.f, 1.f, 1.f, 1.f) });
            renderer->AddPointLight( PointLight{ math::Vec3(0, 5, -5), math::Vec4(1.f, 1.f, 1.f, 1.f) });
            return;
        }
#endif

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [renderer](const ecs::SystemUpdateData& updateData)
        {
            const auto* transforms = updateData.GetComponentArray<const ecs::components::TransformComponent>();
            const auto* pointLights = updateData.GetComponentArray<components::PointLightComponent>();
            const auto& entities = updateData.GetEntities();
            for (size_t i = 0; i < entities.size(); ++i)
            {
                renderer->AddPointLight( PointLight{ transforms[i].pos, pointLights[i].color });
            }
        });
    }
#if SPARK_EDITOR
    void PointLightSystem::OnRender(const ecs::QueryResults& results)
    {
        auto* editor = Application::Get()->GetEditor();
        if (editor->InGameMode())
        {
            return;
        }
        auto renderer = Renderer::Get<Renderer>();
        const auto updateMode = renderer->SupportsMultiThreadedRendering() ? ecs::UpdateMode::MultiThreaded : ecs::UpdateMode::SingleThreaded;
        ecs::ForEachArcheType(results, updateMode, false, [editor, renderer](const ecs::SystemUpdateData& updateData)
        {
            const auto* transforms = updateData.GetComponentArray<const ecs::components::TransformComponent>();
            auto* pointLights = updateData.GetComponentArray<components::PointLightComponent>();
            const auto* activeCamera = updateData.GetSingletonComponent<const camera::ActiveCameraComponent>();
            const auto& entities = updateData.GetEntities();
            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& transform = transforms[i];
                auto& pointLight = pointLights[i];
                if (!pointLight.vertBuffer)
                {
                    auto mesh = geo::util::CreateRectMesh({ -0.5f, -0.5f }, { 0.5f, 0.5f });
                    pointLight.vertBuffer = VertexBuffer::CreateVertexBuffer(mesh);
                    pointLight.vertBuffer->CreatePlatformResource();
                    pointLight.indexBuffer = IndexBuffer::CreateIndexBuffer(mesh);
                    pointLight.indexBuffer->CreatePlatformResource();
                    pointLight.iconMaterial = std::make_shared<MaterialInstance>("/engine_assets/materials/M_BillboardPointlight.sass");
                    asset::AssetReference<asset::Texture> pointLightIcon = "/engine_assets/textures/point_light.sass";
                    pointLight.iconMaterial->SetUniform("Texture", 1, &pointLightIcon);
                }

                pointLight.iconMaterial->SetUniform("uniform_color", 1, &pointLight.color);
                pointLight.iconMaterial->SetUniform("pos", 1, &transform.pos);
                pointLight.iconMaterial->SetUniform("view", 1, &activeCamera->view);
                pointLight.iconMaterial->SetUniform("proj", 1, &activeCamera->proj);

                renderer->Submit<commands::SubmitGeo>(editor->GetOffscreenRenderGroup(), pointLight.iconMaterial, pointLight.vertBuffer, pointLight.indexBuffer, transform.pos);
            }
        });
    }
#endif
}
