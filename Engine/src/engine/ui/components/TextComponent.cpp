#include "spark.h"
#include "TextComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(TextComponent)
        DEFINE_SERIALIZED_MEMBER(fontSize)
        DEFINE_SERIALIZED_MEMBER(alignment)
    DEFINE_SPARK_COMPONENT_END(TextComponent)
}