#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/render/PointLight.h"

namespace se::render::components
{
    class PointLightComponent : public reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(PointLightComponent)

        SPARK_MEMBER(Serialized)
        math::Vec3 color;
    };
}
