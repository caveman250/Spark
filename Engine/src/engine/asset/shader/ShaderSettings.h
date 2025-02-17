#pragma once

#include "spark.h"
#include "engine/math/math.h"

namespace se
{
    typedef std::variant<int, float, math::Vec2, math::Vec3, math::Vec4> ShaderSettingValue;

    class ShaderSettingDefinitionBase
    {
    public:
        virtual ~ShaderSettingDefinitionBase() = default;
        virtual std::string ToGLSL() const = 0;
        virtual ShaderSettingValue GetValue() const = 0;
    };

    template <typename T>
    class ShaderSettingDefinition : public ShaderSettingDefinitionBase
    {
    public:
        ShaderSettingDefinition(const T& value) : m_Value(value) {}
        std::string ToGLSL() const override { return std::format("{}", m_Value); }
        ShaderSettingValue GetValue() const override { return m_Value; }
    private:
        T m_Value;
    };

    class ShaderSettings 
    {
    public:
        bool HasDefinition(const std::string& setting) const;
        std::string GetSettingText(const std::string& setting) const;

        ShaderSettingValue GetSettingValue(const std::string& setting) const;

        template <typename T>
        void SetSetting(const std::string& setting, const T& value);
    private:
        std::unordered_map<std::string, std::shared_ptr<ShaderSettingDefinitionBase>> m_Settings;
    };

    template <typename T>
    void ShaderSettings::SetSetting(const std::string& setting, const T& value)
    {
        SPARK_ASSERT(!m_Settings.contains(setting));
        m_Settings[setting] = std::make_shared<ShaderSettingDefinition<T>>(value);
    }
}
