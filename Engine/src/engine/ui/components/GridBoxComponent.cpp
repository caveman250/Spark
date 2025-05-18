#include "Widgets.generated.h"
#include "engine/ui/util/GridBoxUtil.h"
#include "GridBoxComponent.h"

namespace se::ui
{
    se::math::IntVec2 GetGridBoxChildrenDesiredSize(ecs::System* system,
                                                        const ecs::Id& entity,
                                                        const ui::components::RectTransformComponent& parentRect,
                                                        const ui::components::RectTransformComponent& thisRect,
                                                        const components::GridBoxComponent* context)
    {
        auto world = Application::Get()->GetWorld();
        math::IntVec2 itemSize = util::GetGridBoxItemSize(entity, system, thisRect);

        math::IntVec2 desiredSize = { context->spacing, context->spacing };
        auto numChildren = world->GetChildren(entity).size();
        for (size_t i = 0; i < numChildren; ++i)
        {
            if (desiredSize.x + itemSize.x + context->spacing > parentRect.rect.size.x)
            {
                desiredSize.x = 0;
                desiredSize.y += itemSize.y + context->spacing;
            }
            else
            {
                desiredSize.x += itemSize.x + context->spacing;
            }
        }

        desiredSize.y += itemSize.y + context->spacing;
        return desiredSize;
    }
}
