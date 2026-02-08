#include "spark.h"

#include "MeshRenderSystem.h"
#include "engine/Application.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/render/MaterialInstance.h"
#include "engine/render/Renderer.h"
#include "engine/asset/mesh/Model.h"
#include "engine/ecs/util/SystemUtil.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/singleton_components/MeshRenderComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::render::systems
{
    ecs::SystemDeclaration MeshRenderSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("MeshRenderSystem")
                    .WithComponent<const TransformComponent>()
                    .WithComponent<MeshComponent>()
                    .WithSingletonComponent<const camera::ActiveCameraComponent>()
                    .WithSingletonComponent<singleton_components::MeshRenderComponent>();
    }

    ecs::UpdateMode MeshRenderSystem::GetUpdateMode() const
    {
        auto renderer = Renderer::Get<Renderer>();
        return renderer->SupportsMultiThreadedRendering() ? ecs::UpdateMode::MultiThreaded : ecs::UpdateMode::SingleThreaded;
    }

    void MeshRenderSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        auto* meshes = updateData.GetComponentArray<MeshComponent>();
        const auto* transforms = updateData.GetComponentArray<const TransformComponent>();
        const auto* camera = updateData.GetSingletonComponent<const camera::ActiveCameraComponent>();
        auto* meshRenderComp = updateData.GetSingletonComponent<singleton_components::MeshRenderComponent>();

        for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
        {
            auto& mesh = meshes[i];
            size_t modelHash = std::hash<asset::AssetReference<asset::Model>>()(mesh.model);
            bool buffersValid = mesh.vertBuffer && mesh.modelHash == modelHash;
#if SPARK_EDITOR
            if (std::ranges::contains(meshRenderComp->invalidatedMeshAssets, mesh.model))
            {
                buffersValid = false;
            }
#endif

            if (!buffersValid)
            {
                if (!mesh.model.IsSet())
                {
                    continue;
                }
                const auto& modelAsset = mesh.model.GetAsset();
                auto staticMesh = modelAsset->GetMesh();
                mesh.vertBuffer = VertexBuffer::CreateVertexBuffer(staticMesh);
                mesh.vertBuffer->CreatePlatformResource();
                mesh.indexBuffer = IndexBuffer::CreateIndexBuffer(staticMesh);
                mesh.indexBuffer->CreatePlatformResource();
                mesh.modelHash = modelHash;
            }

            if (!mesh.materialInstance || mesh.materialInstance->GetMaterial() != mesh.material)
            {
                if (mesh.material.IsSet())
                {
                    mesh.materialInstance = MaterialInstance::CreateMaterialInstance(mesh.material.GetAsset());
                }
                else if (mesh.model.IsSet() && mesh.model.GetAsset()->HasMaterial())
                {
                    mesh.materialInstance = MaterialInstance::CreateMaterialInstance(mesh.model.GetAsset()->GetMaterial());
                }
            }

            if (const auto& material = mesh.materialInstance)
            {
                material->SetUniform("model", asset::shader::ast::AstType::Mat4, 1, &transforms[i].worldTransform);
                material->SetUniform("view", asset::shader::ast::AstType::Mat4, 1, &camera->view);
                material->SetUniform("proj", asset::shader::ast::AstType::Mat4, 1, &camera->proj);
            }
        }
    }

    void MeshRenderSystem::OnRender(const ecs::SystemUpdateData& updateData)
    {
        auto renderer = Renderer::Get<Renderer>();
        const auto* meshes = updateData.GetComponentArray<MeshComponent>();
        auto* meshRenderComp = updateData.GetSingletonComponent<singleton_components::MeshRenderComponent>();

#if SPARK_EDITOR
        size_t defaultRenderGroup = Application::Get()->GetEditorRuntime()->GetOffscreenRenderGroup();
#else
        size_t defaultRenderGroup = renderer->GetDefaultRenderGroup();
#endif

        for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
        {
            const auto& meshComp = meshes[i];

            size_t renderGroup = defaultRenderGroup;
            if (meshComp.renderLayer != 0)
            {
                meshRenderComp->mutex.lock();
                auto it = meshRenderComp->layerRenderGroups.find(meshComp.renderLayer);
                if (it == meshRenderComp->layerRenderGroups.end())
                {
                    it = meshRenderComp->layerRenderGroups.insert(std::make_pair(meshComp.renderLayer, renderer->AllocRenderGroup(meshComp.renderLayer))).first;
#if SPARK_EDITOR
                    renderer->SetFrameBuffer(it->second, Application::Get()->GetEditorRuntime()->GetFrameBuffer());
#endif
                }
                meshRenderComp->mutex.unlock();

                renderGroup = it->second;
            }

            if (meshComp.materialInstance && meshComp.vertBuffer && meshComp.indexBuffer)
            {
                renderer->Submit<commands::SubmitGeo>(renderGroup, meshComp.materialInstance, meshComp.vertBuffer, meshComp.indexBuffer);
            }
        }
    }
}