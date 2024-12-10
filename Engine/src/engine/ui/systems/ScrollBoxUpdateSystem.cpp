#include "ScrollBoxUpdateSystem.h"

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
    DEFINE_SPARK_SYSTEM(ScrollBoxUpdateSystem)

    void ScrollBoxUpdateSystem::OnRender(const std::vector<ecs::Id>& entities,
                                         const components::ScrollBoxComponent* scrollBoxes,
                                         components::RectTransformComponent* rectTransforms,
                                         const components::ReceivesMouseEventsComponent* mouseComps)
    {
        PROFILE_SCOPE("ScrollBoxUpdateSystem::OnRender")

        auto renderer = render::Renderer::Get();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& rect = rectTransforms[i];
            const auto& mouseComp = mouseComps[i];

            if (mouseComp.hovered)
            {
                for (const auto& mouseEVent : mouseComp.mouseEvents)
                {
                    if (mouseEVent.scrollDelta != 0)
                    {
                        RunChildQuery<components::RectTransformComponent>(entity, [mouseEVent](const std::vector<ecs::Id>& children, components::RectTransformComponent* childTransforms)
                        {
                            for (size_t i = 0; i < children.size(); ++i)
                            {
                                auto& childRect = childTransforms[i];
                                childRect.minY += mouseEVent.scrollDelta * 10;
                                childRect.maxY += mouseEVent.scrollDelta * 10;
                            }

                            return true;
                        });
                       rect.needsLayout = true;
                    }
                }
            }
        }
    }
}
