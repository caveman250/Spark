module;
#include "engine/ecs/Component.h"

export module Spark.Editor:SingletonComponents;
import Spark.Math;

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
