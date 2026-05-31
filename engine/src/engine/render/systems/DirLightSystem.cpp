#include "DirLightSystem.h"

#include "engine/render/Renderer.h"
#include "engine/render/components/DirLightComponent.h"
#include "engine/render/MaterialInstance.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/geo/util/MeshUtil.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/IndexBuffer.h"
#include "engine/render/MaterialInstance.h"
#include "engine/asset/mesh/Model.h"

namespace se::render::systems
{
    ecs::SystemDeclaration DirLightSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("DirLightSystem")
                .WithComponent<components::DirLightComponent>()
                .WithComponent<const ecs::components::TransformComponent>()
                .WithSingletonComponent<const camera::ActiveCameraComponent>();
    }

    void DirLightSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto renderer = Renderer::Get<Renderer>();

#if SPARK_EDITOR
        auto* editor = Application::Get()->GetEditor();
        if (editor->GetMode() == editor::EditorMode::Prefab)
        {
            return;
        }
#endif

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [renderer](const ecs::SystemUpdateData& updateData)
        {
            const auto* transforms = updateData.GetComponentArray<const ecs::components::TransformComponent>();
            const auto* dirLights = updateData.GetComponentArray<components::DirLightComponent>();
            const auto& entities = updateData.GetEntities();
            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& transform = transforms[i];

                math::Vec3 forward(transform.transform[2][0], transform.transform[2][1], transform.transform[2][2]);

                renderer->AddDirLight( DirLight{ forward, dirLights[i].color });
            }
        });
    }

    #if SPARK_EDITOR
    void DirLightSystem::OnRender(const ecs::QueryResults& results)
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
            auto* dirLights = updateData.GetComponentArray<components::DirLightComponent>();
            const auto* activeCamera = updateData.GetSingletonComponent<const camera::ActiveCameraComponent>();
            const auto& entities = updateData.GetEntities();
            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& transform = transforms[i];
                auto& dirLight = dirLights[i];
                if (!dirLight.vertBuffer)
                {
                    auto mesh = geo::util::CreateRectMesh({ -0.5f, -0.5f }, { 0.5f, 0.5f });
                    dirLight.vertBuffer = VertexBuffer::CreateVertexBuffer(mesh);
                    dirLight.vertBuffer->CreatePlatformResource();
                    dirLight.indexBuffer = IndexBuffer::CreateIndexBuffer(mesh);
                    dirLight.indexBuffer->CreatePlatformResource();
                    dirLight.iconMaterial = std::make_shared<MaterialInstance>("/engine_assets/materials/M_BillboardPointlight.sass");
                    asset::AssetReference<asset::Texture> pointLightIcon = "/engine_assets/textures/point_light.sass";
                    dirLight.iconMaterial->SetUniform("Texture", 1, &pointLightIcon);
                }

                dirLight.iconMaterial->SetUniform("uniform_color", 1, &dirLight.color);
                dirLight.iconMaterial->SetUniform("pos", 1, &transform.pos);
                dirLight.iconMaterial->SetUniform("view", 1, &activeCamera->view);
                dirLight.iconMaterial->SetUniform("proj", 1, &activeCamera->proj);

                renderer->Submit<commands::SubmitGeo>(editor->GetOffscreenRenderGroup(), dirLight.iconMaterial, dirLight.vertBuffer, dirLight.indexBuffer, transform.pos);

                if (!dirLight.guideVertBuffer)
                {
                    auto mesh = asset::AssetManager::Get()->GetAsset<asset::Model>("/engine_assets/models/dir_light_guide.sass");
                    dirLight.guideVertBuffer = VertexBuffer::CreateVertexBuffer(mesh->GetMesh());
                    dirLight.guideVertBuffer->CreatePlatformResource();
                    dirLight.guideIndexBuffer = IndexBuffer::CreateIndexBuffer(mesh->GetMesh());
                    dirLight.guideIndexBuffer->CreatePlatformResource();
                    dirLight.guideMaterial = std::make_shared<MaterialInstance>("/engine_assets/materials/M_DirLightGuide.sass");
                }

                math::Mat3 inverseModel = transforms[i].worldTransform;
                inverseModel = math::Inverse(inverseModel);
                dirLight.guideMaterial->SetUniform("inverseModel", 1, &inverseModel, true);
                dirLight.guideMaterial->SetUniform("model", 1, &transforms[i].worldTransform, true);
                dirLight.guideMaterial->SetUniform("view", 1, &activeCamera->view, true);
                dirLight.guideMaterial->SetUniform("proj", 1, &activeCamera->proj, true);
                renderer->Submit<commands::SubmitGeo>(editor->GetOffscreenRenderGroup(), dirLight.guideMaterial, dirLight.guideVertBuffer, dirLight.guideIndexBuffer, transform.pos);
            }
        });
    }
#endif
}
