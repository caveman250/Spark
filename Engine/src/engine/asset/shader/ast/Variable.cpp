#include "Variable.h"

namespace se::asset::shader::ast
{
DEFINE_SPARK_CLASS_BEGIN(Variable)
    DEFINE_MEMBER(type)
    DEFINE_MEMBER(arraySizeVariable)
    DEFINE_MEMBER(arraySizeConstant)
DEFINE_SPARK_CLASS_END()
}