module Spark.Math.Viewportutil;

namespace se::math::util
{
    Vec3 ScreenToWorldPoint(const Vec2& screenPos,
                          const Mat4& view,
                          const Mat4& projection,
                          const Vec4& viewport,
                          float depth)
    {
        Vec3 win(screenPos.x, viewport.w - screenPos.y, depth);
        Vec3 point = UnProject(win, view, projection, viewport);
        return point;
    }
}