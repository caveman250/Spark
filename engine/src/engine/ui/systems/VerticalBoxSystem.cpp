#include "spark.h"

#include "VerticalBoxSystem.h"
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
    void VerticalBoxSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("VerticalBoxSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();
        const auto& entities = updateData.GetEntities();
        auto* rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();
        auto* verticalBoxes = updateData.GetComponentArray<ui::components::VerticalBoxComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& verticalBoxTransform = rectTransforms[i];
            auto& verticalBox = verticalBoxes[i];

            if (verticalBoxTransform.needsLayout)
            {
                verticalBox.dirty |= verticalBoxTransform.rect.size != verticalBoxTransform.lastRect.size;
                if (!verticalBox.dirty)
                {
                    auto posDelta = verticalBoxTransform.rect.topLeft - verticalBoxTransform.lastRect.topLeft;
                    util::TranslateChildren(entity, this, posDelta);
                    verticalBoxTransform.needsLayout = false;
                }
                else
                {
                    // need to apply width before asking for desired size or things like grid boxes will layout as if they have a single column
                    auto dec = ecs::ChildQueryDeclaration()
                            .WithComponent<components::RectTransformComponent>();
                    RunChildQuery(entity, dec,
                       [verticalBoxTransform](const ecs::SystemUpdateData& updateData)
                       {
                           const auto& children = updateData.GetEntities();
                           auto* rects = updateData.GetComponentArray<components::RectTransformComponent>();

                           for (size_t i = 0; i < children.size(); ++i)
                           {
                               auto& rect = rects[i];
                               rect.anchors = { 0.f, 1.f, 0.f, 0.f };
                               rect.rect = ui::util::CalculateScreenSpaceRect(rect, verticalBoxTransform);
                           }

                           return false;
                       });

                    auto childRects = util::GetChildrenDesiredSizes(entity, this, verticalBoxTransform);
                    int currY = verticalBox.paddingTop;
                    auto window = Application::Get()->GetPrimaryWindow();
                    for (const auto& child : world->GetChildren(entity))
                    {
                        auto desiredSizeInfo = childRects.at(child);

                        desiredSizeInfo.rectTransform->anchors = {0.f, 1.f, 0.f, 0.f};
                        desiredSizeInfo.rectTransform->minY = currY;
                        desiredSizeInfo.rectTransform->minX = verticalBox.paddingLeft;
                        desiredSizeInfo.rectTransform->maxX = verticalBox.paddingRight;
                        desiredSizeInfo.rectTransform->maxY = desiredSizeInfo.rectTransform->minY + desiredSizeInfo.desiredSize.y / window->GetContentScale();
                        currY = desiredSizeInfo.rectTransform->maxY + verticalBox.spacing;

                        desiredSizeInfo.rectTransform->rect = util::CalculateScreenSpaceRect(*desiredSizeInfo.rectTransform,
                                                                            verticalBoxTransform);
                        desiredSizeInfo.rectTransform->layer = verticalBoxTransform.layer + 1;

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

                    verticalBoxTransform.rect.size.y = currY * window->GetContentScale() +
                                                    verticalBox.paddingBottom * window->GetContentScale();
                    verticalBoxTransform.maxY = verticalBoxTransform.minY + currY + verticalBox.paddingBottom;

                    verticalBox.dirty = false;
                }
            }


            verticalBoxTransform.needsLayout = false;
        }
    }
}
