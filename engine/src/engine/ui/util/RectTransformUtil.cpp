#include "RectTransformUtil.h"
#include "Widgets.generated.h"
#include "platform/IWindow.h"

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
        auto window = Application::Get()->GetWindow();

        math::IntVec2 parentBottomRight = parentRect.topLeft + parentRect.size;
        int parentWidth = parentBottomRight.x - parentRect.topLeft.x;
        int parentHeight = parentBottomRight.y - parentRect.topLeft.y;

        Rect ret = { };
        if (transform.anchors.left > 0.5f)
        {
            ret.topLeft.x = static_cast<int>((parentRect.topLeft.x + transform.anchors.left * parentWidth) - transform.minX * window->GetContentScale());
        }
        else
        {
            ret.topLeft.x = static_cast<int>((parentRect.topLeft.x + transform.anchors.left * parentWidth) + transform.minX * window->GetContentScale());
        }

        if (transform.anchors.top > 0.5f)
        {
            ret.topLeft.y = static_cast<int>((parentRect.topLeft.y + transform.anchors.top * parentHeight) - transform.minY * window->GetContentScale());
        }
        else
        {
            ret.topLeft.y = static_cast<int>((parentRect.topLeft.y + transform.anchors.top * parentHeight) + transform.minY * window->GetContentScale());
        }

        if (transform.anchors.right > 0.5f)
        {
            ret.size.x = static_cast<int>((parentRect.topLeft.x + transform.anchors.right * parentWidth) - transform.maxX * window->GetContentScale() - ret.topLeft.x);
        }
        else
        {
            ret.size.x = static_cast<int>((parentRect.topLeft.x + transform.anchors.right * parentWidth) + transform.maxX * window->GetContentScale() - ret.topLeft.x);
        }

        if (transform.anchors.bottom > 0.5f)
        {
            ret.size.y = static_cast<int>((parentRect.topLeft.y + transform.anchors.bottom * parentHeight) - transform.maxY * window->GetContentScale() - ret.topLeft.y);
        }
        else
        {
            ret.size.y = static_cast<int>((parentRect.topLeft.y + transform.anchors.bottom * parentHeight) + transform.maxY * window->GetContentScale() - ret.topLeft.y);
        }

        // < = more wide > = more tall
        constexpr float aspectTolerance = 0.01f;
        if (transform.minAspectRatio != 0.f)
        {
            float aspectRatio = (float)ret.size.x / (float)ret.size.y;
            if (aspectRatio < transform.minAspectRatio - aspectTolerance)
            {
                int newWidth = static_cast<int>(ret.size.y * transform.minAspectRatio);
                if (newWidth > ret.size.x)
                {
                    // shrink y
                    int newHeight = static_cast<int>(ret.size.x / transform.minAspectRatio);
                    int delta = ret.size.y - newHeight;
                    ret.topLeft.y += static_cast<int>(delta * 0.5f);
                    ret.size.y = newHeight;
                }
                else
                {
                    // grow x
                    int delta = ret.size.x - newWidth;
                    ret.topLeft.x += static_cast<int>(delta * 0.5f);
                    ret.size.x = newWidth;
                }
            }
        }

        if (transform.maxAspectRatio != 0.f)
        {
            float aspectRatio = (float)ret.size.x / (float)ret.size.y;
            if (aspectRatio > transform.maxAspectRatio + aspectTolerance)
            {
                int newWidth = static_cast<int>(ret.size.y * transform.maxAspectRatio);
                if (newWidth > ret.size.x)
                {
                    // shrink y
                    int newHeight = static_cast<int>(ret.size.x / transform.maxAspectRatio);
                    int delta = ret.size.y - newHeight;
                    ret.topLeft.y += static_cast<int>(delta * 0.5f);
                    ret.size.y = newHeight;
                }
                else
                {
                    // grow x
                    int delta = ret.size.x - newWidth;
                    ret.topLeft.x += static_cast<int>(delta * 0.5f);
                    ret.size.x = newWidth;
                }
            }
        }

        return ret;
    }

    void LayoutChildren(ecs::World* world,
                        ecs::System* system,
                        const ecs::Id& entity,
                        const RectTransformComponent& parentRect,
                        int depth)
    {
        EASY_BLOCK("se::ui::util::LayoutChildren");
        auto declaration = ecs::HeirachyQueryDeclaration()
                .WithComponent<RectTransformComponent>();
        system->RunChildQuery(entity, declaration,
                              [system, world, parentRect, depth](const ecs::SystemUpdateData& updateData)
                              {
                                  const auto& children = updateData.GetEntities();
                                  auto* childTransform = updateData.GetComponentArray<RectTransformComponent>();

                                  for (size_t i = 0; i < children.size(); ++i)
                                  {
                                      RectTransformComponent& child = childTransform[i];
                                      child.rect = CalculateScreenSpaceRect(child, parentRect);
                                      child.layer = depth;

                                      if (!child.overridesChildSizes)
                                      {
                                          if (world->HasComponent<ecs::components::ParentComponent>(children[i]))
                                          {
                                              LayoutChildren(world, system, children[i], child, depth);
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
                                                                              const RectTransformComponent& transform)
    {
        std::unordered_map<ecs::Id, ChildDesiredSizeInfo> ret = {};

        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<RectTransformComponent>()
                .WithComponent<const WidgetComponent>()
                .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(
                        ecs::ComponentMutability::Immutable);

        system->RunChildQuery(entity, dec,
        [system, &ret, transform](const ecs::SystemUpdateData &updateData)
        {
            const auto &children = updateData.GetEntities();
            auto *rectTransforms = updateData.GetComponentArray<RectTransformComponent>();
            auto *widgets = updateData.GetComponentArray<const WidgetComponent>();

            std::visit([system, &widgets, &children, &ret, transform, &rectTransforms](auto &&value)
            {
                for (size_t i = 0; i < children.size(); ++i)
                {
                    const auto &child = children[i];
                    auto &rectTransform = rectTransforms[i];
                    math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system, child, &widgets[i], transform, rectTransform, &value[i]);
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

        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<RectTransformComponent>();

        system->RunChildQuery(entity, dec,
        [&ret](const ecs::SystemUpdateData &updateData)
        {
            const auto &children = updateData.GetEntities();
            auto *rectTransforms = updateData.GetComponentArray<RectTransformComponent>();

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
        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<RectTransformComponent>();
        system->RunRecursiveChildQuery(entity, dec,
                               [delta](const ecs::SystemUpdateData& updateData)
                               {
                                   const auto& children = updateData.GetEntities();
                                   auto* rects = updateData.GetComponentArray<RectTransformComponent>();

                                   for (size_t i = 0; i < children.size(); ++i)
                                   {
                                       auto& rect = rects[i];
                                       rect.rect.topLeft += delta;
                                   }

                                   return false;
                               });
    }

    void InvalidateParent(const ecs::Id& entity, ecs::System* system)
    {
        auto world = Application::Get()->GetWorld();
        auto currentEntity = entity;
        auto dec = ecs::HeirachyQueryDeclaration()
            .WithComponent<RectTransformComponent>();
        auto func = [](const ecs::SystemUpdateData& updateData)
        {
            auto rectTransform = updateData.GetComponentArray<RectTransformComponent>();
            rectTransform->needsLayout = true;
        };

        ecs::Id lastEntity = currentEntity;
        while (currentEntity != ecs::s_InvalidEntity)
        {
            auto parent = world->GetParent(currentEntity);
            if (parent == ecs::s_InvalidEntity)
            {
                system->RunQueryOnParent(lastEntity, dec, func);
            }
            lastEntity = currentEntity;
            currentEntity = parent;
        }
    }
}
