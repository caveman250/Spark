#include "spark.h"

#include "RootRectTransformSystem.h"

#include "LastRectSystem.h"
#include "engine/ui/util/RectTransformUtil.h"

#include "engine/Application.h"
#include <easy/profiler.h>
#include "platform/IWindow.h"
#include "Widgets.generated.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ecs/util/SystemUtil.h"

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

    void RootRectTransformSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("RootRectTransformSystem::OnUpdate");

        auto world = Application::Get()->GetWorld();

        auto window = Application::Get()->GetWindow();
        bool forceRelayout = !math::FloatEqual(window->GetContentScale(), window->GetLastContentScale());
        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [this, world, window, forceRelayout](const ecs::SystemUpdateData& updateData)
        {
            std::visit([this, &updateData, world, window, forceRelayout](auto&& value)
            {
                const auto& entities = updateData.GetEntities();
                auto* transform = updateData.GetComponentArray<components::RectTransformComponent>();

                ecs::util::ParallelForEachEntity(updateData, [this, entities, world, transform, window, value, forceRelayout](size_t i)
                {
                    const auto& entity = entities[i];
                    Rect windowRect = Rect {
                        .topLeft = math::IntVec2(0, 0),
                        .size = ecs::IsEditorEntity(entity) ? math::IntVec2(window->GetWidth(), window->GetHeight()) : Application::Get()->GetGameViewportSize()
                    };

                    if (forceRelayout)
                    {
                        auto dec = ecs::HeirachyQueryDeclaration()
                            .WithComponent<components::RectTransformComponent>();
                        RunRecursiveChildQuery(entity, dec, [](const ecs::SystemUpdateData& updateData)
                        {
                            auto* transform = updateData.GetComponentArray<components::RectTransformComponent>();
                            transform->needsLayout = true;
                            transform->cachedParentSize = {};
                            transform->desiredSize = {};
                            transform->rect = {};
                            return false;
                        });
                    }

                    auto& trans = transform[i];
                    trans.lastRect = trans.rect;
                    trans.rect = util::CalculateScreenSpaceRect(trans, windowRect);
                    if (forceRelayout || trans.needsLayout || trans.rect != trans.lastRect)
                    {
                        Layout::LayoutWidgetChildren(world, this, entities[i], trans, trans.layer, value + i);
                        trans.needsLayout = false;
                    }
                });
            }, updateData.GetVariantComponentArray<SPARK_WIDGET_TYPES_WITH_NULLTYPE>());
        });
    }
}
