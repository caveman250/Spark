#include "Variable.h"

namespace se::asset::shader::ast
{
DEFINE_SPARK_CLASS_BEGIN(Variable)
    DEFINE_SERIALIZED_MEMBER(type)
    DEFINE_SERIALIZED_MEMBER(arraySizeVariable)
    DEFINE_SERIALIZED_MEMBER(arraySizeConstant)
DEFINE_SPARK_CLASS_END(Variable)
}