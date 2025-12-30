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
#include "engine/render/VertexBuffer.h"

using namespace se;
using namespace se::ecs::components;

namespace se::render::systems
{
    ecs::SystemDeclaration MeshRenderSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("MeshRenderSystem")
                    .WithComponent<const TransformComponent>()
                    .WithComponent<MeshComponent>()
                    .WithSingletonComponent<const camera::ActiveCameraComponent>();
    }

    void MeshRenderSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        const auto& entities = updateData.GetEntities();
        auto* meshes = updateData.GetComponentArray<MeshComponent>();
        const auto* transforms = updateData.GetComponentArray<const TransformComponent>();
        const auto* camera = updateData.GetSingletonComponent<const camera::ActiveCameraComponent>();
        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& mesh = meshes[i];
            if (!mesh.model.Loaded())
            {
                const auto& modelAsset = mesh.model.GetAsset();
                auto staticMesh = modelAsset->GetMesh();
                mesh.vertBuffer = VertexBuffer::CreateVertexBuffer(staticMesh);
                mesh.vertBuffer->CreatePlatformResource();
                mesh.indexBuffer = IndexBuffer::CreateIndexBuffer(staticMesh);
                mesh.indexBuffer->CreatePlatformResource();

                if (!mesh.materialInstance)
                {
                    if (mesh.material.IsSet())
                    {
                        mesh.materialInstance = MaterialInstance::CreateMaterialInstance(mesh.material.GetAsset());
                    }
                    else
                    {
                        mesh.materialInstance = MaterialInstance::CreateMaterialInstance(mesh.model.GetAsset()->GetMaterial());
                    }
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

        const auto& entities = updateData.GetEntities();
        const auto* meshes = updateData.GetComponentArray<MeshComponent>();

#if SPARK_EDITOR
        size_t renderGroup = Application::Get()->GetEditorRuntime()->GetOffscreenRenderGroup();
#else
        size_t renderGroup = renderer->GetDefaultRenderGroup();
#endif

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& meshComp = meshes[i];
            if (meshComp.materialInstance && meshComp.vertBuffer && meshComp.indexBuffer)
            {
                renderer->Submit<commands::SubmitGeo>(renderGroup, meshComp.materialInstance, meshComp.vertBuffer, meshComp.indexBuffer);
            }
        }
    }
}