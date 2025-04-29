#include "spark.h"

#include "GridBoxSystem.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/math/Mat4.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/GridBoxUtil.h"
#include "Widgets.generated.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(GridBoxSystem)

    void GridBoxSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("GridBoxSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();
        const auto& entities = updateData.GetEntities();
        auto* rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();
        auto* gridBoxes = updateData.GetComponentArray<ui::components::GridBoxComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& gridBoxTransform = rectTransforms[i];
            auto& gridBox = gridBoxes[i];

            if (gridBoxTransform.needsLayout)
            {
                gridBox.dirty |= gridBoxTransform.rect.size != gridBoxTransform.lastRect.size;
                if (!gridBox.dirty)
                {
                    auto posDelta = gridBoxTransform.rect.topLeft - gridBoxTransform.lastRect.topLeft;
                    util::TranslateChildren(entity, this, posDelta);
                    gridBoxTransform.needsLayout = false;
                }
            }

            if (gridBox.dirty)
            {
                auto itemSize = util::GetGridBoxItemSize(entity, this, gridBoxTransform);
                auto childRects = util::GetChildRectTransforms(entity, this);

                math::IntVec2 cursor = { gridBox.spacing, gridBox.spacing };
                for (const auto& child : world->GetChildren(entity))
                {
                    auto* rectTransform = childRects.at(child);

                    rectTransform->anchors = {0.f, 0.f, 0.f, 0.f};
                    rectTransform->minX = cursor.x;
                    rectTransform->minY = cursor.y;
                    rectTransform->maxX = cursor.x + itemSize.x;
                    rectTransform->maxY = cursor.y + itemSize.y;

                    rectTransform->rect = util::CalculateScreenSpaceRect(*rectTransform,
                                                                        gridBoxTransform);
                    rectTransform->layer = gridBoxTransform.layer + 1;

                    if (!rectTransform->overridesChildSizes)
                    {
                        util::LayoutChildren(world,
                                             this,
                                             child,
                                             *rectTransform,
                                             rectTransform->layer + 1);
                        rectTransform->needsLayout = false;
                    }
                    else
                    {
                        rectTransform->needsLayout = true;
                    }

                    cursor.x += itemSize.x + gridBox.spacing;
                    if (cursor.x + itemSize.x + gridBox.spacing > gridBoxTransform.rect.size.x)
                    {
                        cursor.x = gridBox.spacing;
                        cursor.y += itemSize.y + gridBox.spacing;
                    }
                }

                gridBox.dirty = false;
            }

            gridBoxTransform.needsLayout = false;
        }
    }
}
