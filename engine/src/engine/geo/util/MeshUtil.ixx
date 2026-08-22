module;

#include "spark.h"

export module Spark.Geo.Util.MeshUtil;
import Spark.Asset.Mesh.StaticMesh;
import Spark.Math;

namespace se::geo::util
{
    asset::StaticMesh CreateRectMesh(const math::Vec2& topLeft, const math::Vec2& size);
    asset::StaticMesh CreateCircleMesh(float radius, size_t steps, size_t startIndex = 0, size_t endIndex = std::numeric_limits<size_t>::max());
    asset::StaticMesh CreateCircleMesh(float radius, float innerRadius, size_t steps, size_t startIndex = 0, size_t endIndex = std::numeric_limits<size_t>::max());
}
