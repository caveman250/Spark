#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "RootRectTransformSystem.h"
#include "engine/ui/util/RectTransformUtil.h"

#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(RootRectTransformSystem)

    void RootRectTransformSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("RootRectTransformSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();
        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<ui::components::RectTransformComponent>();
        auto window = Application::Get()->GetPrimaryWindow();
        Rect windowRect = Rect { .topLeft = math::IntVec2(0, 0), .size = math::IntVec2(window->GetWidth(), window->GetHeight()) };

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& trans = transform[i];
            trans.lastRect = trans.rect;
            trans.rect = util::CalculateScreenSpaceRect(trans, windowRect);
            if (trans.rect != trans.lastRect)
            {
                if (trans.rect.size == trans.lastRect.size)
                {
                    util::TranslateChildren(entity, this, trans.rect.topLeft - trans.lastRect.topLeft);
                }
                else if (!trans.overridesChildSizes)
                {
                    util::LayoutChildren(world, this, entities[i], trans, trans.layer + 1);
                }
                else
                {
                    trans.needsLayout = true;
                }
            }
        }
    }
}
