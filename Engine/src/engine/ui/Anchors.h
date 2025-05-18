#pragma once
#include "engine/reflect/Reflect.h"

namespace se::ui
{
    struct Anchors
    {
        SPARK_POD_CLASS(Anchors)

        SPARK_MEMBER(Serialized)
        float left = 0.f;

        SPARK_MEMBER(Serialized)
        float right = 0.f;

        SPARK_MEMBER(Serialized)
        float top = 0.f;

        SPARK_MEMBER(Serialized)
        float bottom = 0.f;
    };
}
