#pragma once

#include "engine/ui/Rect.h"
#include "engine/ecs/System.h"

namespace se::ui
{
    class DesiredSizeCalculator
    {
    public:
        template<typename T>
        static inline math::Vec2 GetDesiredSize(ecs::System* system, const ecs::Id& entity, const Rect& parentRect, const T*)
        {
            return GetChildrenDesiredSize(system, entity, parentRect);
        }

        static math::Vec2 GetChildrenDesiredSize(ecs::System* system, const ecs::Id& entity, const Rect&);
    };
}