#pragma once
#include "engine/ecs/Component.h"
#include "engine/math/Vec3.h"

namespace se::editor::singleton_components
{
    struct EditorPickResult
    {
        ecs::Id entity = {};
        math::Vec3 pos = {};
    };
    class EditorPickComponent : public ecs::Component
    {
        SPARK_SINGLETON_COMPONENT(EditorOnly)

        std::vector<EditorPickResult> results = {};
    };
}
