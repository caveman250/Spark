#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/render/MaterialInstance.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/render/Renderer.h"
#include "MeshRenderSystem.h"

#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::render::systems
{
    DEFINE_SPARK_SYSTEM(MeshRenderSystem)

    void MeshRenderSystem::OnUpdate(const std::vector<ecs::Id>& entities, TransformComponent* transform,
                              const MeshComponent* mesh, const camera::ActiveCameraComponent* camera)
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            if (const auto& material =  mesh[i].materialInstance)
            {
                material->SetUniform("model", asset::shader::ast::AstType::Mat4, 1, &transform[i].worldTransform);
                material->SetUniform("view", asset::shader::ast::AstType::Mat4, 1, &camera->view);
                material->SetUniform("proj", asset::shader::ast::AstType::Mat4, 1, &camera->proj);
            }
        }
    }

    void MeshRenderSystem::OnRender(const std::vector<ecs::Id>& entities, TransformComponent*, const MeshComponent* mesh, const camera::ActiveCameraComponent*)
    {
        auto renderer = render::Renderer::Get<render::Renderer>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& meshComp = mesh[i];
            if (meshComp.materialInstance && meshComp.vertBuffer && meshComp.indexBuffer)
            {
                renderer->Submit<render::commands::SubmitGeo>(meshComp.materialInstance, meshComp.vertBuffer, meshComp.indexBuffer);
            }
        }
    }
}