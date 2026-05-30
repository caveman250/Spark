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
                bool buffersValid = mesh.vertexBuffer.get();

    #if SPARK_EDITOR
                if (std::ranges::contains(meshRenderComp->invalidatedMeshAssets, mesh.model))
                {
                    buffersValid = false;
                }
    #endif

                if (buffersValid && mesh.model.IsSet() && mesh.model.GetAsset()->GetVertexBuffer() != mesh.vertexBuffer)
                {
                    buffersValid = false;
                }

                if (!buffersValid)
                {
                    EASY_BLOCK("Create Buffers")
                    if (!mesh.model.IsSet())
                    {
                        continue;
                    }

                    const auto& modelAsset = mesh.model.GetAsset();
                    const auto& vertBuffer = modelAsset->GetVertexBuffer();
                    const auto& indexBuffer = modelAsset->GetIndexBuffer();

                    if (!vertBuffer.get())
                    {
                        modelAsset->LockBufferMutex(true);
                        const auto& staticMesh = modelAsset->GetMesh();
                        auto newVertBuffer = VertexBuffer::CreateVertexBuffer(staticMesh);
                        newVertBuffer->CreatePlatformResource();
                        modelAsset->SetVertexBuffer(newVertBuffer);
                        auto newIndexBuffer = IndexBuffer::CreateIndexBuffer(staticMesh);
                        newIndexBuffer->CreatePlatformResource();
                        modelAsset->SetIndexBuffer(newIndexBuffer);
                        modelAsset->LockBufferMutex(false);

                        mesh.vertexBuffer = newVertBuffer;
                        mesh.indexBuffer = newIndexBuffer;
                        mesh.aabb = staticMesh.aabb;
                    }
                    else
                    {
                        mesh.vertexBuffer = vertBuffer;
                        mesh.indexBuffer = indexBuffer;
                        mesh.aabb = modelAsset->GetMesh().aabb;
                    }
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
                    transform.aabb = mesh.aabb;

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
            auto* transforms = updateData.GetComponentArray<TransformComponent>();
            auto* meshRenderComp = updateData.GetSingletonComponent<singleton_components::MeshRenderComponent>();

    #if SPARK_EDITOR
            auto* editor = Application::Get()->GetEditor();
            const size_t defaultRenderGroup = editor->GetOffscreenRenderGroup();
    #else
            const size_t defaultRenderGroup = renderer->GetDefaultRenderGroup();
    #endif

            const auto& entities = updateData.GetEntities();
            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& transform = transforms[i];
                const auto& meshComp = meshes[i];

                if (!meshComp.visible)
                {
                    continue;
                }

                size_t baseRenderGroup = defaultRenderGroup;
#if SPARK_EDITOR
                const ecs::Id& scene = *entities[i].scene;
                if (scene == editor->GetPrefabEditorScene() ||
                    (scene == editor->GetEditorScene() && editor->GetMode() == editor::EditorMode::Prefab))
                {
                    baseRenderGroup = editor->GetPrefabRenderGroup();
                }
#endif
                if (meshComp.renderLayer != 0)
                {
                    meshRenderComp->mutex.lock();
                    singleton_components::MeshRenderComponent::LayerKey key = std::make_pair(meshComp.renderLayer, baseRenderGroup);
                    auto it = meshRenderComp->layerRenderGroups.find(key);
                    if (it == meshRenderComp->layerRenderGroups.end())
                    {
                        it = meshRenderComp->layerRenderGroups.insert(std::make_pair(key, renderer->AllocRenderGroup(meshComp.renderLayer))).first;

                        const auto& fb = renderer->GetFrameBuffer(baseRenderGroup);
                        if (fb)
                        {
                            renderer->SetFrameBuffer(it->second, fb);
                        }
                    }
                    meshRenderComp->mutex.unlock();

                    baseRenderGroup = it->second;
                }

                if (meshComp.materialInstance && meshComp.vertexBuffer && meshComp.indexBuffer)
                {
                    renderer->Submit<commands::SubmitGeo>(baseRenderGroup, meshComp.materialInstance, meshComp.vertexBuffer, meshComp.indexBuffer, transform.pos);
                }
            }
        });
    }
}