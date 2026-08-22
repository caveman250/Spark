module;

#include "spark.h"
#include "engine/reflect/Object.h"

export module Spark.Asset.Shader:ShaderSettingDefinitionBase;
import Reflect;
import Spark.Math;

namespace se
{
    export typedef std::variant<int, float, math::Vec2, math::Vec3, math::Vec4, math::Mat3, math::Mat4> ShaderSettingValue;

    export class ShaderSettingDefinitionBase : public reflect::ObjectBase
    {
        SPARK_CLASS(Abstract)
    public:
        virtual ~ShaderSettingDefinitionBase() = default;
        ShaderSettingDefinitionBase() = default;
        ShaderSettingDefinitionBase(bool _internal) : internal(_internal) {}
        virtual std::string ToGLSL() const = 0;
        virtual ShaderSettingValue GetValue() const = 0;

        bool internal = false;
    };
}