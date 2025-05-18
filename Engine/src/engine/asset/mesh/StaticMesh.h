#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/math.h"

namespace se::asset
{
    struct StaticMesh : reflect::ObjectBase
    {
        SPARK_CLASS(StaticMesh)

        SPARK_MEMBER(Serialized)
        std::vector<math::Vec3> vertices;

        SPARK_MEMBER(Serialized)
        std::vector<math::Vec2> uvs;

        SPARK_MEMBER(Serialized)
        std::vector<math::Vec3> normals;

        SPARK_MEMBER(Serialized)
        std::vector<uint32_t> indices;
    };
}
