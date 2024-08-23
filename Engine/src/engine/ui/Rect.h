#pragma once
#include "engine/math/IntVec2.h"
#include "engine/reflect/Reflect.h"

namespace se::ui
{
    class Rect 
    {
        DECLARE_SPARK_POD_CLASS(Rect)

        math::IntVec2 topLeft;
        math::IntVec2 size;

        bool operator==(const Rect& rhs) const
        {
            return topLeft == rhs.topLeft && size == rhs.size;
        }

        bool Overlaps(const Rect& bb) const;
    };
}
