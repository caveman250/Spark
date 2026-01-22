#pragma once

#include "engine/ecs/Component.h"

using namespace se;

namespace se::editor::components
{
    struct EditorCameraComponent : ecs::Component
    {
        SPARK_COMPONENT(EditorOnly)

        SPARK_MEMBER()
        bool mouseDown = false;
    };
}