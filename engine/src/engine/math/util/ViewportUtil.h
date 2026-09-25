#pragma once

#include "engine/math/math.h"

namespace se::math::util
{
    Vec3 ScreenRay(const Vec2& screenPos,
                    const Vec3& cameraPos,
                    const Mat4& view,
                    const Mat4& projection,
                    const Vec4& viewport);

    Vec3 ScreenToWorldPoint(const Vec2& screenPos,
                        const Vec3& cameraPos,
                        const Mat4& view,
                        const Mat4& projection,
                        const Vec4& viewport,
                        float planeY = 0.0f);
}
