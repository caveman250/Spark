#include "RectTransformUtil.h"
#include "Widgets.generated.h"

namespace se::ui::util
{
    Rect CalculateScreenSpaceRect(const RectTransformComponent& transform,
                                  const RectTransformComponent& parentTransform)
    {
        return CalculateScreenSpaceRect(transform, parentTransform.rect);
    }

    Rect CalculateScreenSpaceRect(const RectTransformComponent& transform,
                                  const Rect& parentRect)
    {
        math::IntVec2 parentBottomRight = parentRect.topLeft + parentRect.size;
        int parentWidth = parentBottomRight.x - parentRect.topLeft.x;
        int parentHeight = parentBottomRight.y - parentRect.topLeft.y;

        Rect ret = { };
        if (transform.anchors.left > 0.5f)
        {
            ret.topLeft.x = static_cast<int>((parentRect.topLeft.x + transform.anchors.left * parentWidth) - transform.minX);
        }
        else
        {
            ret.topLeft.x = static_cast<int>((parentRect.topLeft.x + transform.anchors.left * parentWidth) + transform.minX);
        }

        if (transform.anchors.top > 0.5f)
        {
            ret.topLeft.y = static_cast<int>((parentRect.topLeft.y + transform.anchors.top * parentHeight) - transform.minY);
        }
        else
        {
            ret.topLeft.y = static_cast<int>((parentRect.topLeft.y + transform.anchors.top * parentHeight) + transform.minY);
        }

        if (transform.anchors.right > 0.5f)
        {
            ret.size.x = static_cast<int>((parentRect.topLeft.x + transform.anchors.right * parentWidth) - transform.maxX - ret.topLeft.x);
        }
        else
        {
            ret.size.x = static_cast<int>((parentRect.topLeft.x + transform.anchors.right * parentWidth) + transform.maxX - ret.topLeft.x);
        }

        if (transform.anchors.bottom > 0.5f)
        {
            ret.size.y = static_cast<int>((parentRect.topLeft.y + transform.anchors.bottom * parentHeight) - transform.maxY - ret.topLeft.y);
        }
        else
        {
            ret.size.y = static_cast<int>((parentRect.topLeft.y + transform.anchors.bottom * parentHeight) + transform.maxY - ret.topLeft.y);
        }

        // < = more wide > = more tall
        constexpr float aspectTolerance = 0.01f;
        if (transform.minAspectRatio != 0.f)
        {
            float aspectRatio = (float)ret.size.x / (float)ret.size.y;
            if (aspectRatio < transform.minAspectRatio - aspectTolerance)
            {
                int newWidth = ret.size.y * transform.minAspectRatio;
                if (newWidth > ret.size.x)
                {
                    // shrink y
                    int newHeight = ret.size.x / transform.minAspectRatio;
                    int delta = ret.size.y - newHeight;
                    ret.topLeft.y += delta * 0.5f;
                    ret.size.y = newHeight;
                }
                else
                {
                    // grow x
                    int delta = ret.size.x - newWidth;
                    ret.topLeft.x += delta * 0.5f;
                    ret.size.x = newWidth;
                }
            }
        }

        if (transform.maxAspectRatio != 0.f)
        {
            float aspectRatio = (float)ret.size.x / (float)ret.size.y;
            if (aspectRatio > transform.maxAspectRatio + aspectTolerance)
            {
                int newWidth = ret.size.y * transform.maxAspectRatio;
                if (newWidth > ret.size.x)
                {
                    // shrink y
                    int newHeight = ret.size.x / transform.maxAspectRatio;
                    int delta = ret.size.y - newHeight;
                    ret.topLeft.y += delta * 0.5f;
                    ret.size.y = newHeight;
                }
                else
                {
                    // grow x
                    int delta = ret.size.x - newWidth;
                    ret.topLeft.x += delta * 0.5f;
                    ret.size.x = newWidth;
                }
            }
        }

        return ret;
    }

    void LayoutChildren(ecs::World* world,
                        ecs::System* system,
                        ecs::Id entity,
                        const RectTransformComponent& parentRect,
                        int depth)
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
                                      if (*children[i].name == "Vector Editor BG")
                                      {
                                          int lol =1;
                                      }

                                      components::RectTransformComponent& child = childTransform[i];
                                      child.rect = util::CalculateScreenSpaceRect(child, parentRect);
                                      child.layer = depth;

                                      if (!child.overridesChildSizes)
                                      {
                                          if (world->HasComponent<ecs::components::ParentComponent>(children[i]))
                                          {
                                              LayoutChildren(world, system, children[i], child, depth + 1);
                                          }

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

    std::unordered_map<ecs::Id, ChildDesiredSizeInfo> GetChildrenDesiredSizes(const ecs::Id& entity,
                                                                              ecs::System* system,
                                                                              const ui::components::RectTransformComponent& transform)
    {
        std::unordered_map<ecs::Id, ChildDesiredSizeInfo> ret = {};

        auto dec = ecs::ChildQueryDeclaration()
                .WithComponent<ui::components::RectTransformComponent>()
                .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(
                        ecs::ComponentMutability::Immutable);

        system->RunChildQuery(entity, dec,
        [system, &ret, transform](const ecs::SystemUpdateData &updateData)
        {
            const auto &children = updateData.GetEntities();
            auto *rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();

            std::visit([system, &children, &ret, transform, &rectTransforms](auto &&value)
            {
                for (size_t i = 0; i < children.size(); ++i)
                {
                    const auto &child = children[i];
                    auto &rectTransform = rectTransforms[i];
                    math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system, child, transform, rectTransform, &value[i]);

                    ret[child] = ChildDesiredSizeInfo(&rectTransform, childDesiredSize);
                }
            },updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());
          return false;
        });
        
        return ret;
    }

    std::unordered_map<ecs::Id, RectTransformComponent*> GetChildRectTransforms(const ecs::Id& entity, ecs::System* system)
    {
        std::unordered_map<ecs::Id, RectTransformComponent*> ret = {};

        auto dec = ecs::ChildQueryDeclaration()
                .WithComponent<ui::components::RectTransformComponent>();

        system->RunChildQuery(entity, dec,
        [&ret](const ecs::SystemUpdateData &updateData)
        {
            const auto &children = updateData.GetEntities();
            auto *rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();

            for (size_t i = 0; i < children.size(); ++i)
            {
                const auto &child = children[i];
                auto &rectTransform = rectTransforms[i];
                ret[child] = &rectTransform;
            }
            return false;
        });

        return ret;
    }

    void TranslateChildren(const ecs::Id& entity, ecs::System* system, const math::IntVec2& delta)
    {
        auto dec = ecs::ChildQueryDeclaration()
                .WithComponent<components::RectTransformComponent>();
        system->RunRecursiveChildQuery(entity, dec,
                               [delta](const ecs::SystemUpdateData& updateData)
                               {
                                   const auto& children = updateData.GetEntities();
                                   auto* rects = updateData.GetComponentArray<components::RectTransformComponent>();

                                   for (size_t i = 0; i < children.size(); ++i)
                                   {
                                       auto& rect = rects[i];
                                       rect.rect.topLeft += delta;
                                   }

                                   return false;
                               });
    }
}
