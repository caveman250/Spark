#include "RectTransformUtil.h"

namespace se::ui::util
{
    Rect CalculateScreenSpaceRect(const RectTransformComponent& transform, const RectTransformComponent& parentTransform)
    {
        math::IntVec2 parentBottomRight = parentTransform.rect.topLeft + parentTransform.rect.size;
        int parentWidth = parentBottomRight.x - parentTransform.rect.topLeft.x;
        int parentHeight = parentBottomRight.y - parentTransform.rect.topLeft.y;

        Rect ret = {};
        if (transform.anchors.left > 0.5f)
        {
            ret.topLeft.x = static_cast<int>((parentTransform.rect.topLeft.x + transform.anchors.left * parentWidth) - transform.minX);
        }
        else
        {
            ret.topLeft.x = static_cast<int>((parentTransform.rect.topLeft.x + transform.anchors.left * parentWidth) + transform.minX);
        }

        if (transform.anchors.top > 0.5f)
        {
            ret.topLeft.y = static_cast<int>((parentTransform.rect.topLeft.y + transform.anchors.top * parentHeight) - transform.minY);
        }
        else
        {
            ret.topLeft.y = static_cast<int>((parentTransform.rect.topLeft.y + transform.anchors.top * parentHeight) + transform.minY);
        }

        if (transform.anchors.right > 0.5f)
        {
            ret.size.x = static_cast<int>((parentTransform.rect.topLeft.x + transform.anchors.right * parentWidth) - transform.maxX - ret.topLeft.x);
        }
        else
        {
            ret.size.x = static_cast<int>((parentTransform.rect.topLeft.x + transform.anchors.right * parentWidth) + transform.maxX - ret.topLeft.x);
        }

        if (transform.anchors.bottom > 0.5f)
        {
            ret.size.y = static_cast<int>((parentTransform.rect.topLeft.y + transform.anchors.bottom * parentHeight) - transform.maxY - ret.topLeft.y);
        }
        else
        {
            ret.size.y = static_cast<int>((parentTransform.rect.topLeft.y + transform.anchors.bottom * parentHeight) + transform.maxY - ret.topLeft.y);
        }
        return ret;
    }

    void LayoutChildren(ecs::World *world, ecs::System* system, ecs::Id entity, const RectTransformComponent &parentRect, int depth)
    {
        auto declaration = ecs::ChildQueryDeclaration()
                .WithComponent<components::RectTransformComponent>();
        system->RunChildQuery(entity, declaration,
        [system, world, parentRect, depth](const ecs::SystemUpdateData& updateData)
        {
            const auto& children = updateData.GetEntities();
            auto* childTransform = updateData.GetComponentArray<ui::components::RectTransformComponent>();

            for (size_t i = 0; i < children.size(); ++i)
            {
                components::RectTransformComponent& child = childTransform[i];
                child.rect = util::CalculateScreenSpaceRect(child, parentRect);
                child.layer = depth;

                if (!child.overridesChildSizes)
                {
                    if (world->HasComponent<ecs::components::ParentComponent>(children[i]))
                    {
                        LayoutChildren(world, system, children[i], child, depth + 1);
                    }

                    child.lastRect = child.rect;
                    child.needsLayout = false;
                }
                else
                {
                    child.needsLayout = true;
                }
            }

            return false;
        });
    }
}
