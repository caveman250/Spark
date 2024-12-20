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

    void ScrollBoxUpdateSystem::OnUpdate(const std::vector<ecs::Id>& entities,
                                         components::ScrollBoxComponent* scrollBoxes,
                                         components::RectTransformComponent* rectTransforms,
                                         const components::ReceivesMouseEventsComponent* mouseComps)
    {
        PROFILE_SCOPE("ScrollBoxUpdateSystem::OnUpdate")

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
                        std::vector<components::RectTransformComponent*> childRects;
                        int minChildY = INT_MAX;
                        int maxChildY = 0;
                        RunChildQuery<components::RectTransformComponent>(entity, [&childRects, &maxChildY, &minChildY](const std::vector<ecs::Id>& children, components::RectTransformComponent* childTransforms)
                        {
                            for (size_t i = 0; i < children.size(); ++i)
                            {
                                auto& childTransform = childTransforms[i];
                                minChildY = std::min(childTransform.rect.topLeft.y, minChildY);
                                maxChildY = std::max(childTransform.rect.topLeft.y + childTransform.rect.size.y, maxChildY);
                                childRects.emplace_back(&childTransform);
                            }

                            return true;
                        });

                        int scrollBoxMaxY = rect.rect.topLeft.y + rect.rect.size.y;

                        int availableScrollSpaceTop = rect.rect.topLeft.y - minChildY;
                        int availableScrollSpaceBottom = maxChildY - scrollBoxMaxY;

                        if (mouseEVent.scrollDelta > 0 && availableScrollSpaceBottom > 0)
                        {
                            int delta = std::min(10, availableScrollSpaceBottom);
                            for (auto& childRect : childRects)
                            {
                                childRect->minY -= mouseEVent.scrollDelta * delta;
                                childRect->maxY -= mouseEVent.scrollDelta * delta;
                            }
                        }
                        else if (mouseEVent.scrollDelta < 0 && availableScrollSpaceTop > 0)
                        {
                            int delta = std::min(10, availableScrollSpaceTop);
                            for (auto& childRect : childRects)
                            {
                                childRect->minY -= mouseEVent.scrollDelta * delta;
                                childRect->maxY -= mouseEVent.scrollDelta * delta;
                            }
                        }

                       rect.needsLayout = true;
                    }
                }
            }
        }
    }
}
