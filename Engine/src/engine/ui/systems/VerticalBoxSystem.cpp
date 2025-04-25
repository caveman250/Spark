#include "spark.h"

#include "VerticalBoxSystem.h"
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
    DEFINE_SPARK_SYSTEM(VerticalBoxSystem)

    void VerticalBoxSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("VerticalBoxSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();
        const auto& entities = updateData.GetEntities();
        auto* rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();
        auto* verticalBoxes = updateData.GetComponentArray<ui::components::VerticalBoxComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& verticalBoxTransform = rectTransforms[i];
            auto& verticalBox = verticalBoxes[i];

            if (verticalBoxTransform.needsLayout)
            {
                verticalBox.dirty |= verticalBoxTransform.rect.size != verticalBoxTransform.lastRect.size;
                if (!verticalBox.dirty)
                {
                    auto posDelta = verticalBoxTransform.rect.topLeft - verticalBoxTransform.lastRect.topLeft;
                    util::TranslateChildren(entity, this, posDelta);
                    verticalBoxTransform.needsLayout = false;
                }
            }

            if (verticalBox.dirty)
            {
                auto childRects = util::GetChildrenDesiredSizes(entity, this, verticalBoxTransform);
                int currY = 0;
                for (const auto& child : world->GetChildren(entity))
                {
                    auto desiredSizeInfo = childRects.at(child);

                    desiredSizeInfo.rectTransform->anchors = {0.f, 1.f, 0.f, 0.f};
                    desiredSizeInfo.rectTransform->minY = currY;
                    desiredSizeInfo.rectTransform->maxY = desiredSizeInfo.rectTransform->minY + desiredSizeInfo.desiredSize.y;
                    currY = desiredSizeInfo.rectTransform->maxY + verticalBox.spacing;

                    desiredSizeInfo.rectTransform->rect = util::CalculateScreenSpaceRect(*desiredSizeInfo.rectTransform,
                                                                        verticalBoxTransform);
                    desiredSizeInfo.rectTransform->layer = verticalBoxTransform.layer + 1;
                    desiredSizeInfo.rectTransform->lastRect = desiredSizeInfo.rectTransform->rect;

                    util::LayoutChildren(world, this, child, *desiredSizeInfo.rectTransform,
                                         desiredSizeInfo.rectTransform->layer + 1);
                }

                verticalBoxTransform.rect.size.y = currY;
                verticalBoxTransform.maxY = verticalBoxTransform.minY + verticalBoxTransform.rect.size.y;
                verticalBox.dirty = false;
            }

            verticalBoxTransform.needsLayout = false;
        }
    }
}
