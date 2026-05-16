#pragma once

#include "engine/geo/Ray.h"
#include "engine/math/math.h"

namespace se::camera
{
    struct ActiveCameraComponent;
}

namespace se::input
{
    struct InputComponent;
}

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

    geo::Ray GetEditorMouseRay(const input::InputComponent* inputComp,
                               const camera::ActiveCameraComponent* cameraComp);
}
