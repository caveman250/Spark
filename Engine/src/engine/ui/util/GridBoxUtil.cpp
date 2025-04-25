#include "GridBoxUtil.h"
#include "engine/ecs/SystemDeclaration.h"
#include "Widgets.generated.h"

namespace se::ui::util
{
    math::IntVec2 GetGridBoxItemSize(const ecs::Id &entity,
                                     ecs::System *system,
                                     const ui::components::RectTransformComponent& rectTransform)
    {
        se::math::IntVec2 itemSize = {};
        auto dec = ecs::ChildQueryDeclaration()
                .WithComponent<const ui::components::RectTransformComponent>()
                .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);

        system->RunChildQuery(entity, dec,
        [rectTransform, &itemSize, system](const ecs::SystemUpdateData& updateData)
        {
            std::visit([rectTransform, updateData, &itemSize, system](auto&& value)
            {
                const auto& entities = updateData.GetEntities();
                const auto& transforms = updateData.GetComponentArray<const ui::components::RectTransformComponent>();
                for (size_t i = 0; i < entities.size(); ++i)
                {
                    math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system,
                                                                                            entities[i],
                                                                                           rectTransform,
                                                                                            transforms[i],
                                                                                            value);

                    itemSize.x = std::max(itemSize.x, childDesiredSize.x);
                    itemSize.y = std::max(itemSize.y, childDesiredSize.y);
                }
            }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());

          return false;
        });

        return itemSize;
    }
}