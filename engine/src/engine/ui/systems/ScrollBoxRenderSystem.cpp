#include "ScrollBoxRenderSystem.h"

#include "engine/math/Mat4.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/Renderer.h"
#include "platform/IWindow.h"
#include "engine/render/RenderCommand.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void ScrollBoxRenderSystem::OnRender(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("ScrollBoxRenderSystem::OnRender")

        auto renderer = render::Renderer::Get<render::Renderer>();

        const auto& entities = updateData.GetEntities();
        const auto* rectTransforms = updateData.GetComponentArray<const components::RectTransformComponent>();
        auto* renderComp = updateData.GetSingletonComponent<singleton_components::UIRenderComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            const auto& rect = rectTransforms[i];
            renderComp->entityPreRenderCommands[entity].push_back(renderer->AllocRenderCommand<render::commands::PushScissor>(rect.rect));
            renderComp->entityPostRenderCommands[entity].push_back(renderer->AllocRenderCommand<render::commands::PopScissor>());
        }
    }
}
