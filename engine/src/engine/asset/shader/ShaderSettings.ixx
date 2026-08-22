module;

#include "spark.h"
#include "engine/reflect/Object.h"

export module Spark.Asset.Shader:ShaderSettings;
import :ShaderSettingDefinitionBase;

namespace se
{
    namespace asset::shader::ast
    {
        enum class AstType;
    }

    export template <typename T>
    class ShaderSettingDefinition : public ShaderSettingDefinitionBase
    {
        SPARK_CLASS_TEMPLATED()
    public:
        ShaderSettingDefinition() = default;
        ShaderSettingDefinition(const T& value, bool internal) : ShaderSettingDefinitionBase(internal), m_Value(value) {}
        std::string ToGLSL() const override { return std::format("{}", m_Value); }
        ShaderSettingValue GetValue() const override { return m_Value; }
    private:
        SPARK_MEMBER(Serialized)
        T m_Value = {};
    };

    SPARK_INSTANTIATE_TEMPLATE(ShaderSettingDefinition, int)
    SPARK_INSTANTIATE_TEMPLATE(ShaderSettingDefinition, float)
    SPARK_INSTANTIATE_TEMPLATE(ShaderSettingDefinition, math::Vec3)
    SPARK_INSTANTIATE_TEMPLATE(ShaderSettingDefinition, math::Vec4)

    export class ShaderSettings : public reflect::ObjectBase
    {
        SPARK_CLASS()
    public:
        bool HasDefinition(const std::string& setting) const;
        std::string GetSettingText(const std::string& setting) const;

        ShaderSettingValue GetSettingValue(const std::string& setting) const;

        template <typename T>
        void SetSetting(const std::string& setting, const T& value, bool internal = false);

        void SetSettingDefault(const std::string& setting, asset::shader::ast::AstType type);
    private:
        SPARK_MEMBER(Serialized)
        std::unordered_map<std::string, std::shared_ptr<ShaderSettingDefinitionBase>> m_Settings;
    };

    template <typename T>
    void ShaderSettings::SetSetting(const std::string& setting, const T& value, bool internal)
    {
        // if you add extra types make sure to add them to the template instantiations for ShaderSettingDefinition above
        static_assert(std::is_same_v<T, float> ||
            std::is_same_v<T, int> ||
            std::is_same_v<T, math::Vec3> ||
            std::is_same_v<T, math::Vec4>);
        m_Settings[setting] = std::make_shared<ShaderSettingDefinition<T>>(value, internal);
    }
}

#include "se_ShaderSettingDefinition.generated.h"
