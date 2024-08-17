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
                RunChildQuery<components::RectTransformComponent>(entities[i], [world, trans](const std::vector<ecs::Id>& children, components::RectTransformComponent* childTransform)
                {
                    for (size_t i = 0; i < children.size(); ++i)
                    {
                        components::RectTransformComponent& child = childTransform[i];
                        float parentWidth = trans.rect.bottomRight.x - trans.rect.topLeft.x;
                        float parentHeight = trans.rect.bottomRight.y - trans.rect.topLeft.y;

                        child.rect.topLeft = { trans.rect.topLeft.x + child.minX + child.anchors.left * parentWidth,
                                                    trans.rect.topLeft.y + child.minY + child.anchors.top * parentHeight };

                        if (child.anchors.right > 0)
                        {
                            child.rect.bottomRight.x = trans.rect.bottomRight.x - child.maxX - ((1.f - child.anchors.right) * parentWidth);
                        }
                        else
                        {
                            child.rect.bottomRight.x = trans.rect.topLeft.x + child.maxX;
                        }

                        if (child.anchors.bottom > 0)
                        {
                            child.rect.bottomRight.y = trans.rect.bottomRight.y - child.maxY - ((1.f - child.anchors.bottom) * parentHeight);
                        }
                        else
                        {
                            child.rect.bottomRight.y = trans.rect.topLeft.y + child.maxX;
                        }

                        child.needsLayout = world->HasComponent<ParentComponent>(children[i]);
                    }
                });

                trans.needsLayout = false;
            }
        }
    }
}
