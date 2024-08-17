#include "spark.h"

#include <engine/math/Mat4.h>
#include <engine/ecs/components/MeshComponent.h>
#include "RootRectTransformSystem.h"

#include "engine/Application.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(RootRectTransformSystem)

    void RootRectTransformSystem::OnUpdate(const std::vector<ecs::Id>& entities, components::RectTransformComponent* transform, const RootComponent*)
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& trans = transform[i];
            trans.rect = {{ trans.minX, trans.minY }, { trans.maxX, trans.maxY }};
            if (trans.rect != trans.lastRect)
            {
                trans.needsLayout = true;
            }

            trans.lastRect = trans.rect;
        }
    }
}
