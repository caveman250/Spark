#pragma once

#include "engine/math/IntVec2.h"

namespace se::editor::util
{
    math::IntVec2 ScreenSpaceToGameViewportSpace(int mouseX, int mouseY);
    bool PosWithinViewport(int mouseX, int mouseY);
}