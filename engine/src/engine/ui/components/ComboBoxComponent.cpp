#include "ComboBoxComponent.h"
#include "engine/ui/util/WidgetUtil.h"

namespace se::ui
{
    template <>
    void Layout::LayoutWidgetChildren<components::ComboBoxComponent>(ecs::World* world,
                                     ecs::System* system,
                                     const ecs::Id& entity,
                                     components::RectTransformComponent& comboBoxRect,
                                     int layer,
                                     components::ComboBoxComponent* comboBox)
    {
        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<components::RectTransformComponent>();

        util::ForEachWidgetChild(entity, system, dec, [world, comboBoxRect, comboBox, layer, system](const ecs::SystemUpdateData& updateData, auto&& widgetComp)
        {
            const auto& entity = updateData.GetEntity();
            auto* rectTransform = updateData.GetComponentArray<components::RectTransformComponent>();

            int childLayer = 0;
            if (entity == comboBox->collapsedEntity)
            {
                rectTransform->rect.topLeft = comboBoxRect.rect.topLeft;
                rectTransform->rect.size.x = comboBoxRect.rect.size.x;
                rectTransform->rect.size.y = DesiredSizeCalculator::GetDesiredSize(system, entity, *rectTransform, widgetComp).y;
                childLayer = layer;
            }
            else if (entity == comboBox->expandedEntity)
            {
                rectTransform->rect.topLeft = comboBoxRect.rect.topLeft;
                rectTransform->rect.size.x = comboBoxRect.rect.size.x;
                rectTransform->rect.size.y = DesiredSizeCalculator::GetDesiredSize(system, entity, *rectTransform, widgetComp).y;
                rectTransform->layer = -1;
                childLayer = -1;
            }
            else
            {
                SPARK_ASSERT(false, "Unexpected ComboBox child.");
            }

            LayoutWidgetChildren(world, system, entity, *rectTransform, childLayer, widgetComp);
            rectTransform->needsLayout = false;
        });

        comboBoxRect.needsLayout = false;
    }
}
