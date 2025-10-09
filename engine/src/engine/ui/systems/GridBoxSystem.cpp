#include "spark.h"

#include "GridBoxSystem.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/GridBoxUtil.h"
#include "Widgets.generated.h"
#include "RectTransformSystem.h"
#include "ScrollViewUpdateSystem.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration GridBoxSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Grid Box System")
                    .WithComponent<components::GridBoxComponent>()
                    .WithComponent<components::RectTransformComponent>()
                    .WithHeirachyQuery<const components::WidgetComponent>()
                    .WithDependency<RectTransformSystem>()
                    .WithDependency<ScrollViewUpdateSystem>()
                    .WithHeirachyQuerys<SPARK_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);
    }

    void GridBoxSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("GridBoxSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();
        auto window = Application::Get()->GetWindow();
        const auto& entities = updateData.GetEntities();
        auto* rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
        auto* gridBoxes = updateData.GetComponentArray<components::GridBoxComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& gridBoxTransform = rectTransforms[i];
            auto& gridBox = gridBoxes[i];

            if (gridBoxTransform.needsLayout)
            {
                // make sure our anchors are set correctly so that desired size works as expected.
                auto dec = ecs::HeirachyQueryDeclaration()
                        .WithComponent<components::RectTransformComponent>();
                RunChildQuery(entity, dec,
                [](const ecs::SystemUpdateData& updateData)
                {
                    const auto& children = updateData.GetEntities();
                    auto* rects = updateData.GetComponentArray<components::RectTransformComponent>();

                    for (size_t i = 0; i < children.size(); ++i)
                    {
                        auto& rect = rects[i];
                        rect.anchors = { 0.f, 1.f, 0.f, 0.f };
                        rect.minY = 0;
                        rect.maxY = 0;
                        rect.minX = 0;
                        rect.maxX = 0;
                    }

                    return false;
                });

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
                    rectTransform->layer = gridBoxTransform.layer;

                    if (!rectTransform->overridesChildSizes)
                    {
                        util::LayoutChildren(world,
                                             this,
                                             child,
                                             *rectTransform,
                                             rectTransform->layer);
                        rectTransform->needsLayout = false;
                    }
                    else
                    {
                        rectTransform->needsLayout = true;
                    }

                    cursor.x += itemSize.x + gridBox.spacing;
                    if (cursor.x + itemSize.x + gridBox.spacing > gridBoxTransform.rect.size.x / window->GetContentScale())
                    {
                        cursor.x = gridBox.spacing;
                        cursor.y += itemSize.y + gridBox.spacing;
                    }
                }

                gridBoxTransform.rect.size.y = static_cast<int>(cursor.y * window->GetContentScale());
                gridBoxTransform.maxY = gridBoxTransform.minY + cursor.y;
            }

            gridBoxTransform.needsLayout = false;
        }
    }
}
