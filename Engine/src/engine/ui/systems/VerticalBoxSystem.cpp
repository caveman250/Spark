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

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& verticalBoxTransform = rectTransforms[i];

            if (verticalBoxTransform.needsLayout)
            {
                int currY = 0;

                auto dec = ecs::ChildQueryDeclaration()
                    .WithComponent<ui::components::RectTransformComponent>()
                    .WithVariantComponent<SPARK_CONST_WIDGET_TYPES>(ecs::ComponentMutability::Immutable);

                RunChildQuery(entity, dec,
                   [this, &currY, world, verticalBoxTransform](const ecs::SystemUpdateData& updateData)
                   {
                       const auto& children = updateData.GetEntities();
                       auto* rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();

                       std::visit([this, &currY, world, children, verticalBoxTransform, rectTransforms](auto&& value)
                       {
                           for (size_t i = 0; i < children.size(); ++i)
                           {
                               const auto& child = children[i];
                               auto& rectTransform = rectTransforms[i];
                               rectTransform.anchors = { 0.f, 1.f, 0.f, 0.f };
                               rectTransform.minY = currY;
                               math::Vec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(
                                       this, child, rectTransform.rect,
                                       &value[i]);
                               currY = rectTransform.maxY = rectTransform.minY + childDesiredSize.y;

                               rectTransform.rect = util::CalculateScreenSpaceRect(rectTransform, verticalBoxTransform);
                               rectTransform.layer = verticalBoxTransform.layer + 1;
                               rectTransform.lastRect = rectTransform.rect;

                               util::LayoutChildren(world, this, child, rectTransform, rectTransform.layer + 1);
                           }
                       }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES>());
                       return false;
                   });
            }
        }
    }
}
