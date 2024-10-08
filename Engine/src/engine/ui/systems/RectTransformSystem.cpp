#include "spark.h"

#include <engine/math/Mat4.h>
#include <engine/ecs/components/MeshComponent.h>
#include "RectTransformSystem.h"

#include "engine/Application.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(RectTransformSystem)

    void RectTransformSystem::OnUpdate(const std::vector<ecs::Id>& entities, components::RectTransformComponent* transform, ParentComponent*)
    {
        PROFILE_SCOPE("RectTransformSystem::OnUpdate")
        auto world = Application::Get()->GetWorld();
        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& trans = transform[i];
            if (trans.needsLayout)
            {
                LayoutChildren(world, entities[i], trans, trans.layer + 1);
                trans.needsLayout = false;
            }
        }
    }

    void RectTransformSystem::LayoutChildren(ecs::World* world, ecs::Id entity, const components::RectTransformComponent &parentRect, int depth)
    {
        RunChildQuery<components::RectTransformComponent>(entity, [this, world, parentRect, depth](const std::vector<ecs::Id>& children, components::RectTransformComponent* childTransform)
        {
            for (size_t i = 0; i < children.size(); ++i)
            {
                components::RectTransformComponent& child = childTransform[i];
                math::IntVec2 parentBottomRight = parentRect.rect.topLeft + parentRect.rect.size;
                int parentWidth = parentBottomRight.x - parentRect.rect.topLeft.x;
                int parentHeight = parentBottomRight.y - parentRect.rect.topLeft.y;

                child.rect.topLeft = { static_cast<int>(parentRect.rect.topLeft.x + child.minX + child.anchors.left * parentWidth),
                                          static_cast<int>(parentRect.rect.topLeft.y + child.minY + child.anchors.top * parentHeight) };

                if (child.anchors.right > 0)
                {
                    child.rect.size.x = static_cast<int>(parentBottomRight.x - child.maxX - ((1.f - child.anchors.right) * (float)parentWidth) - child.rect.topLeft.x);
                }
                else
                {
                    child.rect.size.x = parentRect.rect.topLeft.x + child.maxX - child.rect.topLeft.x;
                }

                if (child.anchors.bottom > 0)
                {
                    child.rect.size.y = static_cast<int>(parentBottomRight.y - child.maxY - ((1.f - child.anchors.bottom) * (float)parentHeight) - child.rect.topLeft.y);
                }
                else
                {
                    child.rect.size.y = parentRect.rect.topLeft.y + child.maxY - child.rect.topLeft.y;
                }

                child.layer = depth;

                if (world->HasComponent<ParentComponent>(children[i]))
                {
                    LayoutChildren(world, children[i], child, depth + 1);
                }
            }
        });
    }
}
