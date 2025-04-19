#include "spark.h"

#include "VerticalBoxSystem.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/math/Mat4.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "Widgets.generated.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(VerticalBoxSystem)

    void VerticalBoxSystem::OnUpdate(const std::vector<ecs::Id>& entities,
                                     components::VerticalBoxComponent*,
                                     components::RectTransformComponent* rectTransforms)
    {
        PROFILE_SCOPE("VerticalBoxSystem::OnUpdate")

        for (size_t i = 0; i < entities.size(); ++i)
        {

            //const auto& entity = entities[i];
            auto& rectTransform = rectTransforms[i];

            if (rectTransform.needsLayout)
            {
//                std::unordered_map<ecs::Id, se::math::Vec2> desiredSizes = {};
//                RunVariantChildQuery<SPARK_CONST_WIDGET_TYPES>(entity,
//                   (std::function<bool(const ecs::Id &, std::variant<SPARK_WIDGET_CONST_POINTER_TYPES>)>)
//                   [this, rectTransform, &desiredSizes](const ecs::Id &child, std::variant<SPARK_WIDGET_CONST_POINTER_TYPES> var)
//                   {
//                       std::visit([this, rectTransform, child, &desiredSizes](
//                               auto &&value)
//                                  {
//                                      math::Vec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(
//                                              this, child, rectTransform.rect,
//                                              value);
//                                      desiredSizes.insert(std::make_pair(child,
//                                                                         childDesiredSize));
//                                  }, var);
//                       return false;
//                   });
//
//                int currY = 0;
//                RunChildQuery<components::RectTransformComponent>(entity,
//                [desiredSizes, &currY](const std::vector<ecs::Id> &children,
//                                     components::RectTransformComponent *childTransforms)
//                {
//                    for (size_t i = 0; i < children.size(); ++i)
//                    {
//                        const auto &child = children[i];
//                        auto &rectTransform = childTransforms[i];
//                        math::Vec2 desiredSize = desiredSizes.at(child);
//
//                        // only respect desired Y
//                        rectTransform.anchors.right = 1.f;
//                        rectTransform.minY = currY;
//                        rectTransform.maxY = currY + desiredSize.y;
//                        rectTransform.needsLayout = true;
//
//                        currY = rectTransform.maxY;
//                    }
//                    return false;
//                });
            }
        }
    }
}
