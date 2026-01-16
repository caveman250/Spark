#include "spark.h"

#include "RootRectTransformSystem.h"

#include "LastRectSystem.h"
#include "engine/ui/util/RectTransformUtil.h"

#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include <easy/profiler.h>
#include "platform/IWindow.h"
#include "Widgets.generated.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration RootRectTransformSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("RootRectTransformSystem")
                .WithComponent<components::RectTransformComponent>()
                .WithComponent<const RootComponent>()
                .WithVariantComponent<SPARK_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Mutable)
                .WithHeirachyQuerys<SPARK_WIDGET_TYPES>(ecs::ComponentMutability::Mutable)
                .WithHeirachyQuery<components::WidgetComponent>()
                .WithDependency<LastRectSystem>();
    }

    void RootRectTransformSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("RootRectTransformSystem::OnUpdate");

        auto world = Application::Get()->GetWorld();
        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<components::RectTransformComponent>();
        auto window = Application::Get()->GetWindow();

        std::visit([this, updateData, entities, world, transform, window](auto&& value)
        {
            ecs::util::ForEachEntity(this, updateData,
                                     [this, entities, world, transform, window, value](size_t i)
                                     {
                                         const auto& entity = entities[i];
                                         Rect windowRect = Rect {
                                             .topLeft = math::IntVec2(0, 0),
                                             .size = ecs::IsEditorEntity(entity) ? math::IntVec2(window->GetWidth(), window->GetHeight()) : Application::Get()->GetGameViewportSize()
                                         };

                                         auto& trans = transform[i];
                                         trans.lastRect = trans.rect;
                                         trans.rect = util::CalculateScreenSpaceRect(trans, windowRect);
                                         if (trans.needsLayout || trans.rect != trans.lastRect)
                                         {
                                             if (!trans.overridesChildSizes)
                                             {
                                                 Layout::LayoutWidgetChildren(world, this, entities[i], trans, value + i);
                                                 trans.needsLayout = false;
                                             }
                                             else
                                             {
                                                 trans.needsLayout = true;
                                             }
                                         }
                                     });
        }, updateData.GetVariantComponentArray<SPARK_WIDGET_TYPES_WITH_NULLTYPE>());
    }
}
