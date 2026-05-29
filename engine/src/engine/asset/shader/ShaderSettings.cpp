#include "ShaderSettings.h"
#include "engine/math/math.h"

namespace se
{
    bool ShaderSettings::HasDefinition(const std::string& setting) const
    {
        return m_Settings.contains(setting);
    }

    std::string ShaderSettings::GetSettingText(const std::string& setting) const
    {
        if (SPARK_VERIFY(m_Settings.contains(setting)))
        {
            return m_Settings.at(setting)->ToGLSL();
        }

        return "";
    }

    ShaderSettingValue ShaderSettings::GetSettingValue(const std::string& setting) const
    {
        if (SPARK_VERIFY(m_Settings.contains(setting)))
        {
            return m_Settings.at(setting)->GetValue();
        }

        return 0;
    }

    void ShaderSettings::SetSettingDefault(const std::string& setting, asset::shader::ast::AstType type)
    {
        switch (type)
        {
            case asset::shader::ast::AstType::Int:
                m_Settings[setting] = std::make_shared<ShaderSettingDefinition<int>>();
                break;
            case asset::shader::ast::AstType::Float:
                m_Settings[setting] = std::make_shared<ShaderSettingDefinition<float>>();
                break;
            case asset::shader::ast::AstType::Vec2:
                m_Settings[setting] = std::make_shared<ShaderSettingDefinition<math::Vec2>>();
                break;
            case asset::shader::ast::AstType::Vec3:
                m_Settings[setting] = std::make_shared<ShaderSettingDefinition<math::Vec3>>();
                break;
            case asset::shader::ast::AstType::Vec4:
                m_Settings[setting] = std::make_shared<ShaderSettingDefinition<math::Vec4>>();
                break;
            case asset::shader::ast::AstType::Mat3:
                m_Settings[setting] = std::make_shared<ShaderSettingDefinition<math::Mat3>>();
                break;
            case asset::shader::ast::AstType::Mat4:
                m_Settings[setting] = std::make_shared<ShaderSettingDefinition<math::Mat4>>();
                break;
            case asset::shader::ast::AstType::Bool:
                m_Settings[setting] = std::make_shared<ShaderSettingDefinition<bool>>();
                break;
            case asset::shader::ast::AstType::None:
            case asset::shader::ast::AstType::Void:
            case asset::shader::ast::AstType::Sampler2D:
            case asset::shader::ast::AstType::Sampler2DReference:
            case asset::shader::ast::AstType::Invalid:
                break;
        }
    }
}
