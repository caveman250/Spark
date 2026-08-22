export module Spark.Math.Viewportutil;
export import Spark.Math;

namespace se::math::util
{
    Vec3 ScreenToWorldPoint(const Vec2& screenPos,
                              const Mat4& view,
                              const Mat4& projection,
                              const Vec4& viewport,
                              float depth = 0.f);
}
