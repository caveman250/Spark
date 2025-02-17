#include "ShaderSettings.h"

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
}
