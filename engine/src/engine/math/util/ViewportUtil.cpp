#include "ViewportUtil.h"

namespace se::math::util
{
    Vec3 ScreenRay(const Vec2& screenPos,
                        const Vec3& cameraPos,
                        const Mat4& view,
                        const Mat4& projection,
                        const Vec4& viewport)
    {
        float flippedY = (viewport[1] + viewport[3]) - screenPos.y;
        Vec3 farPoint = UnProject(Vec3(screenPos.x, flippedY, 1.0f), view, projection, viewport);
        return Normalized(farPoint - cameraPos);
    }

    Vec3 ScreenToWorldPoint(const Vec2& screenPos,
                        const Vec3& cameraPos,
                        const Mat4& view,
                        const Mat4& projection,
                        const Vec4& viewport,
                        float planeY)
    {
        Vec3 rayDir = ScreenRay(screenPos, cameraPos, view, projection, viewport);
        if (std::abs(rayDir.y) < FLT_EPSILON)
        {
            return cameraPos;
        }

        float t = (planeY - cameraPos.y) / rayDir.y;

        return cameraPos + rayDir * t;
    }
}