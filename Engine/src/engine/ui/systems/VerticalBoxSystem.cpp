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
    DEFINE_SPARK_SYSTEM(VerticalBoxSystem)

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
                    verticalBoxTransform.needsLayout = false;
                }
            }

            if (verticalBox.dirty)
            {
                auto dec = ecs::ChildQueryDeclaration()
                        .WithComponent<ui::components::RectTransformComponent>()
                        .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(
                                ecs::ComponentMutability::Immutable);

                std::unordered_map<ecs::Id, std::pair<ui::components::RectTransformComponent*, math::IntVec2>> childRects = {};

                RunChildQuery(entity, dec,
                  [this, &childRects, verticalBoxTransform](const ecs::SystemUpdateData &updateData)
                  {
                      const auto &children = updateData.GetEntities();
                      auto *rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();

                      std::visit([this, &children, &childRects, verticalBoxTransform, &rectTransforms](auto &&value)
                      {
                          for (size_t i = 0; i < children.size(); ++i)
                          {
                              const auto &child = children[i];
                              auto &rectTransform = rectTransforms[i];
                              math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(
                                      this,
                                      child,
                                      verticalBoxTransform,
                                      rectTransform,
                                      &value[i]);

                              childRects[child] = std::make_pair(&rectTransform, childDesiredSize);
                          }
                      },updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());
                      return false;
                  });

                int currY = 0;
                for (const auto& child : world->GetChildren(entity))
                {
                    auto pair = childRects.at(child);
                    components::RectTransformComponent& rectTransform = *pair.first;
                    math::IntVec2 desiredSize = pair.second;

                    rectTransform.anchors = {0.f, 1.f, 0.f, 0.f};
                    rectTransform.minY = currY;
                    rectTransform.maxY = rectTransform.minY + desiredSize.y;
                    currY = rectTransform.maxY + verticalBox.spacing;

                    rectTransform.rect = util::CalculateScreenSpaceRect(rectTransform,
                                                                        verticalBoxTransform);
                    rectTransform.layer = verticalBoxTransform.layer + 1;
                    rectTransform.lastRect = rectTransform.rect;

                    util::LayoutChildren(world, this, child, rectTransform,
                                         rectTransform.layer + 1);
                }

                verticalBoxTransform.rect.size.y = currY;
                verticalBoxTransform.maxY = verticalBoxTransform.minY + verticalBoxTransform.rect.size.y;
                verticalBox.dirty = false;
            }

            verticalBoxTransform.needsLayout = false;
        }
    }
}
