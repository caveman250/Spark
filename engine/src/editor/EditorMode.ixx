module;
#include "engine/reflect/Reflect_fwd.h"

export module EditorMode;

namespace se::editor
{
    SPARK_ENUM()
    export enum class EditorMode
    {
        Default,
        Prefab
    };
}
