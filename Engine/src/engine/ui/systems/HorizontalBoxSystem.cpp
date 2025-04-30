#include "spark.h"

#include "HorizontalBoxSystem.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/math/Mat4.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "Widgets.generated.h"
#include "engine/ui/util/RectTransformUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(HorizontalBoxSystem)

    void HorizontalBoxSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("HorizontalBoxSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();
        const auto& entities = updateData.GetEntities();
        auto* rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();
        auto* horizontalBoxes = updateData.GetComponentArray<ui::components::HorizontalBoxComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& horizontalBoxTransform = rectTransforms[i];
            auto& horizontalBox = horizontalBoxes[i];

            if (horizontalBoxTransform.needsLayout)
            {
                horizontalBox.dirty |= horizontalBoxTransform.rect.size != horizontalBoxTransform.lastRect.size;
                if (!horizontalBox.dirty)
                {
                    auto posDelta = horizontalBoxTransform.rect.topLeft - horizontalBoxTransform.lastRect.topLeft;
                    util::TranslateChildren(entity, this, posDelta);
                    horizontalBoxTransform.needsLayout = false;
                }
            }

            if (horizontalBox.dirty)
            {
                auto childRects = util::GetChildrenDesiredSizes(entity, this, horizontalBoxTransform);
                int currX = 0;
                for (const auto& child : world->GetChildren(entity))
                {
                    auto desiredSizeInfo = childRects.at(child);

                    desiredSizeInfo.rectTransform->anchors = {0.f, 0.f, 0.f, 1.f};
                    desiredSizeInfo.rectTransform->minX = currX;
                    desiredSizeInfo.rectTransform->maxX = desiredSizeInfo.rectTransform->minX + desiredSizeInfo.desiredSize.x;
                    currX = desiredSizeInfo.rectTransform->maxX + horizontalBox.spacing;

                    desiredSizeInfo.rectTransform->rect = util::CalculateScreenSpaceRect(*desiredSizeInfo.rectTransform,
                                                                                         horizontalBoxTransform);
                    desiredSizeInfo.rectTransform->layer = horizontalBoxTransform.layer + 1;

                    if (!desiredSizeInfo.rectTransform->overridesChildSizes)
                    {
                        util::LayoutChildren(world,
                                             this,
                                             child,
                                             *desiredSizeInfo.rectTransform,
                                             desiredSizeInfo.rectTransform->layer + 1);
                        desiredSizeInfo.rectTransform->needsLayout = false;
                    }
                    else
                    {
                        desiredSizeInfo.rectTransform->needsLayout = true;
                    }
                }

                horizontalBoxTransform.rect.size.x = currX;
                horizontalBoxTransform.maxX = horizontalBoxTransform.minX + horizontalBoxTransform.rect.size.x;
                horizontalBox.dirty = false;
            }

            horizontalBoxTransform.needsLayout = false;
        }
    }
}
