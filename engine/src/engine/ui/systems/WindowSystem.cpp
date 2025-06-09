#include "spark.h"

#include "WindowSystem.h"

#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/components/WindowComponent.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void WindowSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("WindowSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        auto* windows = updateData.GetComponentArray<components::WindowComponent>();
        auto* transforms = updateData.GetComponentArray<components::RectTransformComponent>();
        auto* appWindow = Application::Get()->GetPrimaryWindow();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& window = windows[i];
            auto& transform = transforms[i];

            if (window.pendingDeltaX != 0 || window.pendingDeltaY != 0)
            {
                float invContentScale = 1.f / appWindow->GetContentScale();
                transform.minX += window.pendingDeltaX * invContentScale;
                transform.maxX += window.pendingDeltaX * invContentScale;
                transform.minY += window.pendingDeltaY * invContentScale;
                transform.maxY += window.pendingDeltaY * invContentScale;

                math::Vec2 delta = math::Vec2(window.pendingDeltaX, window.pendingDeltaY);
                transform.rect.topLeft += delta;
                util::TranslateChildren(entities[i], this, delta);

                window.pendingDeltaX = 0;
                window.pendingDeltaY = 0;
            }
        }
    }
}
