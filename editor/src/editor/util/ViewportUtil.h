#pragma once

#include "engine/math/math.h"

namespace se::editor::util
{
    math::IntVec2 ScreenSpaceToGameViewportSpace(int mouseX,
                                                 int mouseY);
    bool PosWithinViewport(int mouseX,
                           int mouseY);
    math::Vec3 ScreenToWorldPoint(const math::Vec2& screenPos,
                                  const math::Mat4& view,
                                  const math::Mat4& projection,
                                  const math::Vec4& viewport);
}
