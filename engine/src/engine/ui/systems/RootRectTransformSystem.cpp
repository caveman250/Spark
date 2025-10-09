#include "spark.h"

#include "RootRectTransformSystem.h"

#include "LastRectSystem.h"
#include "engine/ui/util/RectTransformUtil.h"

#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/profiling/Profiler.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration RootRectTransformSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("RootRectTransformSystem")
                    .WithComponent<components::RectTransformComponent>()
                    .WithComponent<const RootComponent>()
                    .WithDependency<LastRectSystem>();
    }

    void RootRectTransformSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("RootRectTransformSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();
        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<components::RectTransformComponent>();
        auto window = Application::Get()->GetWindow();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            Rect windowRect = Rect {
                .topLeft = math::IntVec2(0, 0),
                .size = entity.HasFlag(ecs::IdFlags::Editor) ?
                    math::IntVec2(window->GetWidth(), window->GetHeight()) :
                    Application::Get()->GetGameViewportSize()
            };

            auto& trans = transform[i];
            trans.lastRect = trans.rect;
            trans.rect = util::CalculateScreenSpaceRect(trans, windowRect);
            if (trans.rect != trans.lastRect)
            {
                if (!trans.overridesChildSizes)
                {
                    util::LayoutChildren(world, this, entities[i], trans, trans.layer);
                }
                else
                {
                    trans.needsLayout = true;
                }
            }
        }
    }
}
