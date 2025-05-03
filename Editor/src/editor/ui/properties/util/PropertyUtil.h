#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::editor::ui::properties::util
{
    ecs::Id CreateMissingPropertyEditorText(reflect::Type* type, float leftAnchor);
}
