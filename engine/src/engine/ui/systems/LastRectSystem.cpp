#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "LastRectSystem.h"

#include <engine/ui/util/RectTransformUtil.h>

#include "engine/Application.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void LastRectSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("LastRectSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<ui::components::RectTransformComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& trans = transform[i];
            trans.lastRect = trans.rect;
        }
    }
}
