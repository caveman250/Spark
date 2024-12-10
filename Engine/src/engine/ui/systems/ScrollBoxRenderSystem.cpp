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
    DEFINE_SPARK_SYSTEM(ScrollBoxRenderSystem)

    void ScrollBoxRenderSystem::OnRender(const std::vector<ecs::Id>& entities,
                                         components::ScrollBoxComponent* scrollBoxes,
                                         const components::RectTransformComponent* rectTransforms,
                                         singleton_components::UIRenderComponent* renderComp)
    {
        PROFILE_SCOPE("ScrollBoxRenderSystem::OnRender")

        auto renderer = render::Renderer::Get();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            const auto& rect = rectTransforms[i];
            renderComp->entityPreRenderCommands[entity].push_back(renderer->AllocRenderCommand<render::commands::PushScissor>(rect.rect));
            renderComp->entityPostRenderCommands[entity].push_back(renderer->AllocRenderCommand<render::commands::PopScissor>());
        }
    }
}
