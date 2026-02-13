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

    void ComboBoxSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("ComboBoxSystem::OnUpdate");

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [this](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* comboBoxes = updateData.GetComponentArray<components::ComboBoxComponent>();
            const auto* mouseComps = updateData.GetComponentArray<const components::MouseInputComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                auto& comboBox = comboBoxes[i];
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
            }
        });
    }
}
