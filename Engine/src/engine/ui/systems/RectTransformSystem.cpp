#include "spark.h"

#include <engine/math/Mat4.h>
#include <engine/ecs/components/MeshComponent.h>
#include "RectTransformSystem.h"

#include "engine/Application.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(RectTransformSystem)

    void RectTransformSystem::OnUpdate(const std::vector<ecs::Id>& entities, components::RectTransformComponent* transform, ParentComponent*)
    {
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
                float parentWidth = parentRect.rect.bottomRight.x - parentRect.rect.topLeft.x;
                float parentHeight = parentRect.rect.bottomRight.y - parentRect.rect.topLeft.y;

                child.rect.topLeft = { parentRect.rect.topLeft.x + child.minX + child.anchors.left * parentWidth,
                    parentRect.rect.topLeft.y + child.minY + child.anchors.top * parentHeight };

                if (child.anchors.right > 0)
                {
                    child.rect.bottomRight.x = parentRect.rect.bottomRight.x - child.maxX - ((1.f - child.anchors.right) * parentWidth);
                }
                else
                {
                    child.rect.bottomRight.x = parentRect.rect.topLeft.x + child.maxX;
                }

                if (child.anchors.bottom > 0)
                {
                    child.rect.bottomRight.y = parentRect.rect.bottomRight.y - child.maxY - ((1.f - child.anchors.bottom) * parentHeight);
                }
                else
                {
                    child.rect.bottomRight.y = parentRect.rect.topLeft.y + child.maxX;
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
