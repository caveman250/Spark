#include <Widgets.generated.h>
#include "HorizontalBoxComponent.h"

namespace se::ui
{
    se::math::IntVec2 GetHorizontalBoxChildrenDesiredSize(ecs::System* system,
                                                        const ecs::Id& entity,
                                                        const ui::components::RectTransformComponent& thisRect,
                                                        const components::HorizontalBoxComponent* context)
    {
        se::math::IntVec2 desiredSize = {};
        auto dec = ecs::ChildQueryDeclaration()
                .WithComponent<ui::components::RectTransformComponent>()
                .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);

        system->RunChildQuery(entity, dec,
        [context, &thisRect, &desiredSize, system](const ecs::SystemUpdateData& updateData)
        {
            std::visit([context, &thisRect, updateData, &desiredSize, system](auto&& value)
            {
                const auto& entities = updateData.GetEntities();
                auto transforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();
                for (size_t i = 0; i < entities.size(); ++i)
                {
                    math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system,
                                                                                           entities[i],
                                                                                           thisRect,
                                                                                           transforms[i],
                                                                                           value);
                    desiredSize.y = std::max(childDesiredSize.y, desiredSize.y);
                    desiredSize.x += childDesiredSize.x + context->spacing;
                }
                }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());
            return false;
        });
        return desiredSize;
    }
}
