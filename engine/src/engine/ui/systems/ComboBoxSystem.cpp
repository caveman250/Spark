#include "ComboBoxSystem.h"

#include "RootRectTransformSystem.h"
#include "spark.h"
#include "engine/Application.h"
#include "engine/math/IntVec2.h"
#include <easy/profiler.h>
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/ui/components/ComboBoxComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "UIMouseInputSystem.h"
#include "Widgets.generated.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration ComboBoxSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("ComboBox System")
                    .WithComponent<components::ComboBoxComponent>()
                    .WithComponent<components::RectTransformComponent>()
                    .WithComponent<const components::MouseInputComponent>()
                    .WithHeirachyQuery<components::WidgetComponent>()
                    .WithHeirachyQuerys<SPARK_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable)
                    .WithDependency<UIMouseInputSystem>()
                    .WithDependency<RootRectTransformSystem>();
    }

    void ComboBoxSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("ComboBoxSystem::OnUpdate");

        auto world = Application::Get()->GetWorld();

        const auto& entities = updateData.GetEntities();
        auto* comboBoxes = updateData.GetComponentArray<components::ComboBoxComponent>();
        auto* transforms = updateData.GetComponentArray<components::RectTransformComponent>();
        const auto* mouseComps = updateData.GetComponentArray<const components::MouseInputComponent>();

        ecs::util::ForEachEntity(this, updateData,
        [this, world, entities, comboBoxes, transforms, mouseComps](size_t i)
        {
            const auto& entity = entities[i];
            auto& comboBox = comboBoxes[i];
            auto& transform = transforms[i];
            const auto& mouseInput = mouseComps[i];

            if (mouseInput.hovered)
            {
                for (const auto& mouseEvent: mouseInput.mouseEvents)
                {
                    if (mouseEvent.button == input::MouseButton::Left)
                    {
                        if (mouseEvent.state == input::KeyState::Up)
                        {
                            comboBox.collapsed = !comboBox.collapsed;
                        }
                    }
                }
            }

            if (comboBox.collapsed != comboBox.lastCollapsed)
            {
                comboBox.lastCollapsed = comboBox.collapsed;

                auto declaration = ecs::HeirachyQueryDeclaration()
                            .WithComponent<components::WidgetComponent>();
                RunQueryOnChild(comboBox.expandedEntity, declaration,
                    [&comboBox](const ecs::SystemUpdateData& updateData)
                    {
                        auto* widget = updateData.GetComponentArray<components::WidgetComponent>();
                        widget->visibility = comboBox.collapsed ? Visibility::Collapsed : Visibility::Visible;
                        widget->updateEnabled = !comboBox.collapsed;
                        widget->dirty = true;
                    });
            }

            if (transform.needsLayout)
            {
                auto desiredSizeInfo = util::GetChildrenDesiredSizes(entity, this, transform);
                for (const auto& [child, desired] : desiredSizeInfo)
                {
                    int childlayer = 0;
                    if (child == comboBox.collapsedEntity)
                    {
                        desired.rectTransform->rect.topLeft = transform.rect.topLeft;
                        desired.rectTransform->rect.size.x = transform.rect.size.x;
                        desired.rectTransform->rect.size.y = desired.desiredSize.y;
                        childlayer = desired.rectTransform->layer;
                    }
                    else if (child == comboBox.expandedEntity)
                    {
                        desired.rectTransform->rect.topLeft = transform.rect.topLeft;
                        desired.rectTransform->rect.size.x = transform.rect.size.x;
                        desired.rectTransform->rect.size.y = DesiredSizeCalculator::GetDesiredSize<ecs::NullComponentType>(this, child, *desired.rectTransform, nullptr).y;
                        desired.rectTransform->layer = -1;
                        childlayer = -1;
                    }
                    else
                    {
                        SPARK_ASSERT(false, "Unexpected ComboBox child.");
                    }

                    if (!desired.rectTransform->overridesChildSizes)
                    {
                        util::LayoutChildren(world, this, child, *desired.rectTransform, childlayer);
                        desired.rectTransform->needsLayout = false;
                    }
                    else
                    {
                        desired.rectTransform->needsLayout = true;
                    }
                }

                transform.needsLayout = false;
            }
        });
    }
}
