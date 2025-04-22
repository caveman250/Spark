#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "RootRectTransformSystem.h"
#include "engine/ui/util/RectTransformUtil.h"

#include "engine/Application.h"
#include "engine/profiling/Profiler.h"

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

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& trans = transform[i];
            trans.lastRect = trans.rect;
            trans.rect = {{ trans.minX, trans.minY }, { trans.maxX - trans.minX, trans.maxY - trans.minY }};
            if (trans.rect != trans.lastRect)
            {
                if (trans.rect.size == trans.lastRect.size)
                {
                    auto posDelta = trans.rect.topLeft - trans.lastRect.topLeft;
                    auto dec = ecs::ChildQueryDeclaration()
                        .WithComponent<components::RectTransformComponent>();
                    RunRecursiveChildQuery(entities[i], dec,
                        [posDelta](const ecs::SystemUpdateData& updateData)
                        {
                            const auto& children = updateData.GetEntities();
                            auto* rects = updateData.GetComponentArray<components::RectTransformComponent>();

                            for (size_t i = 0; i < children.size(); ++i)
                            {
                                auto& rect = rects[i];
                                rect.rect.topLeft += posDelta;
                            }

                            return false;
                        });
                }
                else if (!trans.overridesChildSizes)
                {
                    util::LayoutChildren(world, this, entities[i], trans, trans.layer + 1);
                }
            }
        }
    }
}
