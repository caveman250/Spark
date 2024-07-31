#include "Types.h"

DEFINE_SPARK_ENUM_BEGIN(se::asset::shader::ast::AstType)
    DEFINE_ENUM_VALUE(AstType, Float)
    DEFINE_ENUM_VALUE(AstType, Vec2)
    DEFINE_ENUM_VALUE(AstType, Vec3)
    DEFINE_ENUM_VALUE(AstType, Vec4)
    DEFINE_ENUM_VALUE(AstType, Mat3)
    DEFINE_ENUM_VALUE(AstType, Mat4)
    DEFINE_ENUM_VALUE(AstType, Void)
    DEFINE_ENUM_VALUE(AstType, Sampler2D)
    DEFINE_ENUM_VALUE(AstType, Invalid)
DEFINE_SPARK_ENUM_END()
