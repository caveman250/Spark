module;
#include "spark.h"

export module Spark.UI.Rect;
import Spark.Math;

namespace se::ui
{
    export class Rect
    {
        SPARK_POD_CLASS()

        SPARK_MEMBER(Serialized)
        math::IntVec2 topLeft;

        SPARK_MEMBER(Serialized)
        math::IntVec2 size;

        bool operator==(const Rect& rhs) const
        {
            return topLeft == rhs.topLeft && size == rhs.size;
        }

        bool Contains(const math::IntVec2& point) const;
        bool Contains(const Rect& bb) const;
        bool Overlaps(const Rect& bb) const;
    };
}
