module;

#include "spark.h"

export module Spark.Editor.UI.Properties.Util.PropertyUtil;
import Reflect;

namespace se::editor::ui::properties::util
{
    ecs::Id CreateMissingPropertyEditorText(reflect::Type* type, float leftAnchor, int minY);

}
