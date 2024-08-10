#include "spark.h"

#include <engine/math/Mat4.h>
#include <engine/render/Material.h>
#include <engine/asset/shader/ast/Types.h>
#include <engine/ecs/components/MeshComponent.h>
#include <engine/render/Renderer.h>
#include "MeshRenderSystem.h"

#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::render::systems
{
    DEFINE_SPARK_SYSTEM(MeshRenderSystem)

    void MeshRenderSystem::OnUpdate(const std::vector<ecs::EntityId>& entities, TransformComponent* transform,
                              const MeshComponent* mesh, camera::ActiveCameraComponent* camera)
    {
        auto app = Application::Get();
        camera->proj = math::Perspective(math::Radians(45.f), (float)app->GetPrimaryWindow()->GetWidth() / (float)app->GetPrimaryWindow()->GetHeight(),.1f, 100.f);
        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& transformComp = transform[i];

            math::Mat4 model = Translation(transformComp.pos);
            model = model * AxisAngle(math::Vec3(1.0f, 0.0f, 0.0f), transformComp.rot.x);
            model = model * AxisAngle(math::Vec3(0.0f, 1.0f, 0.0f), transformComp.rot.y);
            model = model * AxisAngle(math::Vec3(0.0f, 0.0f, 1.0f), transformComp.rot.z);

            model = model * Scale(transformComp.scale);

            SPARK_ASSERT((float*)&model[0] == &model[0][0]);

            mesh[i].material->SetUniform("model", asset::shader::ast::AstType::Mat4, 1, &model);
            mesh[i].material->SetUniform("view", asset::shader::ast::AstType::Mat4, 1, &camera->view);
            mesh[i].material->SetUniform("proj", asset::shader::ast::AstType::Mat4, 1, &camera->proj);
        }
    }

    void MeshRenderSystem::OnRender(const std::vector<ecs::EntityId>& entities, TransformComponent*, const MeshComponent* mesh, camera::ActiveCameraComponent*)
    {
        auto app = Application::Get();
        auto renderer = render::Renderer::Get();
        auto window = app->GetPrimaryWindow();
        renderer->Submit<render::commands::Clear>(window, true, true);

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& meshComp = mesh[i];
            renderer->Submit<render::commands::SubmitGeo>(window, meshComp.material, meshComp.vertBuffer, 36);
        }
    }
}