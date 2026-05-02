#include "spark.h"

#include "MeshRenderSystem.h"
#include "engine/Application.h"
#include "engine/asset/mesh/Model.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ecs/util/SystemUtil.h"
#include "engine/render/MaterialInstance.h"
#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/singleton_components/MeshRenderComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::render::systems
{
    ecs::SystemDeclaration MeshRenderSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("MeshRenderSystem")
                    .WithComponent<TransformComponent>()
                    .WithComponent<MeshComponent>()
                    .WithSingletonComponent<const camera::ActiveCameraComponent>()
                    .WithSingletonComponent<singleton_components::MeshRenderComponent>();
    }

    void MeshRenderSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("MeshRenderSystem::OnUpdate");
        const auto renderer = Renderer::Get<Renderer>();
        const auto updateMode = renderer->SupportsMultiThreadedRendering() ? ecs::UpdateMode::MultiThreaded : ecs::UpdateMode::SingleThreaded;
        ecs::ForEachArcheType(results, updateMode, false, [](const ecs::SystemUpdateData& updateData)
        {
            auto* meshes = updateData.GetComponentArray<MeshComponent>();
            auto* transforms = updateData.GetComponentArray<TransformComponent>();
            const auto* camera = updateData.GetSingletonComponent<const camera::ActiveCameraComponent>();

#if SPARK_EDITOR
            auto* meshRenderComp = updateData.GetSingletonComponent<singleton_components::MeshRenderComponent>();
#endif

            for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
            {
                auto& mesh = meshes[i];
                auto& transform = transforms[i];
                const auto& modelAsset = mesh.model.GetAsset();
                bool buffersValid = modelAsset->GetVertexBuffer().get();
    #if SPARK_EDITOR
                if (std::ranges::contains(meshRenderComp->invalidatedMeshAssets, mesh.model))
                {
                    buffersValid = false;
                }
    #endif

                if (!buffersValid)
                {
                    EASY_BLOCK("Create Buffers")
                    if (!mesh.model.IsSet())
                    {
                        continue;
                    }
                    modelAsset->LockBufferMutex(true);
                    auto staticMesh = modelAsset->GetMesh();
                    auto vertBuffer = VertexBuffer::CreateVertexBuffer(staticMesh);
                    vertBuffer->CreatePlatformResource();
                    modelAsset->SetVertexBuffer(vertBuffer);
                    auto indexBuffer = IndexBuffer::CreateIndexBuffer(staticMesh);
                    indexBuffer->CreatePlatformResource();
                    modelAsset->SetIndexBuffer(indexBuffer);
                    modelAsset->LockBufferMutex(false);
                    transform.aabb = staticMesh.aabb;
                }

                bool isOutOfDate = !mesh.materialInstance;
                if (!isOutOfDate && mesh.materialInstanceAsset.IsSet())
                {
                    isOutOfDate = mesh.materialInstance != mesh.materialInstanceAsset.GetAsset();
                }
                else if (!isOutOfDate && mesh.materialAsset.IsSet())
                {
                    isOutOfDate = mesh.materialInstance->GetMaterial() != mesh.materialAsset.GetAsset();
                }

                if (isOutOfDate)
                {
                    EASY_BLOCK("Create Material Instance")
                    if (mesh.materialInstanceAsset.IsSet())
                    {
                        mesh.materialInstance = mesh.materialInstanceAsset.GetAsset();
                    }
                    else if (mesh.materialAsset.IsSet())
                    {
                        mesh.materialInstance = std::make_shared<MaterialInstance>(mesh.materialAsset);
                    }
                    else if (mesh.model.IsSet() && mesh.model.GetAsset()->HasMaterial())
                    {
                        mesh.materialInstance = std::make_shared<MaterialInstance>(mesh.model.GetAsset()->GetMaterialReference());
                    }
                }

                if (const auto& material = mesh.materialInstance)
                {
                    EASY_BLOCK("Set Uniforms")
                    material->SetUniform("model", 1, &transforms[i].worldTransform, true);
                    material->SetUniform("view", 1, &camera->view, true);
                    material->SetUniform("proj", 1, &camera->proj, true);
                }
            }
        });
    }

    void MeshRenderSystem::OnRender(const ecs::QueryResults& results)
    {
        EASY_BLOCK("MeshRenderSystem::OnRender");
        auto renderer = Renderer::Get<Renderer>();
        const auto updateMode = renderer->SupportsMultiThreadedRendering() ? ecs::UpdateMode::MultiThreaded : ecs::UpdateMode::SingleThreaded;

        ecs::ForEachArcheType(results, updateMode, false, [renderer](const ecs::SystemUpdateData& updateData)
        {
            const auto* meshes = updateData.GetComponentArray<MeshComponent>();
            auto* meshRenderComp = updateData.GetSingletonComponent<singleton_components::MeshRenderComponent>();

    #if SPARK_EDITOR
            const size_t defaultRenderGroup = Application::Get()->GetEditorRuntime()->GetOffscreenRenderGroup();
    #else
            const size_t defaultRenderGroup = renderer->GetDefaultRenderGroup();
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

                const auto& modelAsset = meshComp.model.GetAsset();
                const auto& vertBuffer = modelAsset->GetVertexBuffer();
                const auto& indexBuffer = modelAsset->GetIndexBuffer();

                if (meshComp.materialInstance && vertBuffer && indexBuffer)
                {
                    renderer->Submit<commands::SubmitGeo>(renderGroup, meshComp.materialInstance, vertBuffer, indexBuffer);
                }
            }
        });
    }
}