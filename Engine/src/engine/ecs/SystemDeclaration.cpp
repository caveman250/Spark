#include "SystemDeclaration.h"

DEFINE_SPARK_ENUM_BEGIN(se::ecs::ComponentMutability)
    DEFINE_ENUM_VALUE(ComponentMutability, Mutable)
    DEFINE_ENUM_VALUE(ComponentMutability, Immutable)
DEFINE_SPARK_ENUM_END()