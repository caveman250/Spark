#pragma once
#include "engine/math/Vec2.h"
#include "engine/reflect/Reflect.h"

namespace se::ui
{
    class FloatRect 
    {
        SPARK_POD_CLASS()

        SPARK_MEMBER(Serialized)
        math::Vec2 topLeft;

        SPARK_MEMBER(Serialized)
        math::Vec2 size;

        bool operator==(const FloatRect& rhs) const
        {
            return topLeft == rhs.topLeft && size == rhs.size;
        }

        bool Contains(const math::Vec2& point) const;
        bool Overlaps(const FloatRect& bb) const;
    };
}
