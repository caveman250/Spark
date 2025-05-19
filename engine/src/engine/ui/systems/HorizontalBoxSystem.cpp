#include "spark.h"

#include "HorizontalBoxSystem.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/math/Mat4.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "Widgets.generated.h"
#include "engine/ui/util/RectTransformUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void HorizontalBoxSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("HorizontalBoxSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();
        auto window = Application::Get()->GetPrimaryWindow();
        const auto& entities = updateData.GetEntities();
        auto* rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();
        auto* horizontalBoxes = updateData.GetComponentArray<ui::components::HorizontalBoxComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& horizontalBoxTransform = rectTransforms[i];
            auto& horizontalBox = horizontalBoxes[i];

            if (horizontalBoxTransform.needsLayout)
            {
                horizontalBox.dirty |= horizontalBoxTransform.rect.size != horizontalBoxTransform.lastRect.size;
                if (!horizontalBox.dirty)
                {
                    auto posDelta = horizontalBoxTransform.rect.topLeft - horizontalBoxTransform.lastRect.topLeft;
                    util::TranslateChildren(entity, this, posDelta);
                    horizontalBoxTransform.needsLayout = false;
                }
            }

            if (horizontalBox.dirty)
            {
                // need to apply height before asking for desired size or things like grid boxes will layout as if they have a single row
                auto dec = ecs::ChildQueryDeclaration()
                        .WithComponent<components::RectTransformComponent>();
                RunChildQuery(entity, dec,
                              [horizontalBoxTransform](const ecs::SystemUpdateData& updateData)
                              {
                                  const auto& children = updateData.GetEntities();
                                  auto* rects = updateData.GetComponentArray<components::RectTransformComponent>();

                                  for (size_t i = 0; i < children.size(); ++i)
                                  {
                                      auto& rect = rects[i];
                                      rect.anchors = { 0.f, 0.f, 0.f, 1.f };
                                      rect.rect = ui::util::CalculateScreenSpaceRect(rect, horizontalBoxTransform);
                                  }

                                  return false;
                              });

                auto childRects = util::GetChildrenDesiredSizes(entity, this, horizontalBoxTransform);
                int currX = 0;
                for (const auto& child : world->GetChildren(entity))
                {
                    auto desiredSizeInfo = childRects.at(child);

                    desiredSizeInfo.rectTransform->anchors = {0.f, 0.f, 0.f, 1.f};
                    desiredSizeInfo.rectTransform->minX = currX;
                    desiredSizeInfo.rectTransform->maxX = static_cast<int>(desiredSizeInfo.rectTransform->minX + desiredSizeInfo.desiredSize.x / window->GetContentScale());
                    currX = desiredSizeInfo.rectTransform->maxX + horizontalBox.spacing;

                    desiredSizeInfo.rectTransform->rect = util::CalculateScreenSpaceRect(*desiredSizeInfo.rectTransform,
                                                                                         horizontalBoxTransform);
                    desiredSizeInfo.rectTransform->layer = horizontalBoxTransform.layer + 1;

                    if (!desiredSizeInfo.rectTransform->overridesChildSizes)
                    {
                        util::LayoutChildren(world,
                                             this,
                                             child,
                                             *desiredSizeInfo.rectTransform,
                                             desiredSizeInfo.rectTransform->layer + 1);
                        desiredSizeInfo.rectTransform->needsLayout = false;
                    }
                    else
                    {
                        desiredSizeInfo.rectTransform->needsLayout = true;
                    }
                }

                horizontalBoxTransform.rect.size.x = static_cast<int>(currX * window->GetContentScale());
                horizontalBoxTransform.maxX = horizontalBoxTransform.minX + currX;

                horizontalBox.dirty = false;
            }

            horizontalBoxTransform.needsLayout = false;
        }
    }
}
