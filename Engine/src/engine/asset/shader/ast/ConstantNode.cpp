#include "ConstantNode.h"

#include <engine/math/Vec2.h>
#include <engine/math/Vec3.h>
#include <engine/math/Vec4.h>

namespace se::asset::shader::ast
{
    using namespace se::math;
    INIT_TEMPLATED_CLASS_REFLECTION_ONE_ARG(ConstantNode, TEMPLATE_TYPES(int));
    INIT_TEMPLATED_CLASS_REFLECTION_ONE_ARG(ConstantNode, TEMPLATE_TYPES(float));
    INIT_TEMPLATED_CLASS_REFLECTION_ONE_ARG(ConstantNode, TEMPLATE_TYPES(Vec2));
    INIT_TEMPLATED_CLASS_REFLECTION_ONE_ARG(ConstantNode, TEMPLATE_TYPES(Vec3));
    INIT_TEMPLATED_CLASS_REFLECTION_ONE_ARG(ConstantNode, TEMPLATE_TYPES(Vec4));
}
