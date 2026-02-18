#include "GridBoxUtil.h"
#include "engine/ecs/SystemDeclaration.h"
#include "Widgets.generated.h"

namespace se::ui::util
{
    math::IntVec2 GetGridBoxItemSize(const ecs::Id &entity,
                                     ecs::System *system,
                                     const RectTransformComponent& rectTransform)
    {
        math::IntVec2 itemSize = {};
        const auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<RectTransformComponent>()
                .WithComponent<const WidgetComponent>()
                .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);

        auto window = Application::Get()->GetWindow();
        system->RunChildQuery(entity, dec,
        [rectTransform, &itemSize, system, window](const ecs::SystemUpdateData& updateData)
        {
            std::visit([rectTransform, updateData, &itemSize, system, window](auto&& value)
            {
                const auto& entities = updateData.GetEntities();
                auto transforms = updateData.GetComponentArray<RectTransformComponent>();
                auto widgets = updateData.GetComponentArray<const WidgetComponent>();
                for (size_t i = 0; i < entities.size(); ++i)
                {
                    const math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system,
                                                                                            entities[i],
                                                                                            &widgets[i],
                                                                                           rectTransform,
                                                                                            transforms[i],
                                                                                            value);

                    itemSize.x = std::max(itemSize.x, static_cast<int>(childDesiredSize.x / window->GetContentScale()));
                    itemSize.y = std::max(itemSize.y, static_cast<int>(childDesiredSize.y / window->GetContentScale()));
                }
            }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());

          return false;
        });

        return itemSize;
    }
}