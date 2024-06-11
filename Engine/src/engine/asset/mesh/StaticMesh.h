#pragma once

#include "spark.h"

namespace se::asset
{
    struct StaticMesh
    {
        DECLARE_SPARK_CLASS(StaticMesh)

        std::vector<math::Vec3> vertices;
        std::vector<math::Vec2> uvs;
        std::vector<math::Vec3> normals;
        std::vector<uint32_t> indices;
    };
}
