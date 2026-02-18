#include "Font.h"
#include "engine/asset/builder/FontBlueprint.h"
#include "engine/asset/texture/Texture.h"

namespace se::asset
{
    const std::string& Font::GetName() const
    {
        return m_Name;
    }

    const std::shared_ptr<Texture>& Font::GetTextureAsset()
    {
        return m_Texture;
    }

    const CharData& Font::GetCharData(const char c) const
    {
        if (!SPARK_VERIFY(m_CharData.contains(c)))
        {
            static CharData nullCharData = {};
            return nullCharData;
        }

        return m_CharData.at(c);
    }

    float Font::GetLineHeight(const int fontSize) const
    {
        const float scale = static_cast<float>(fontSize) / builder::FontBlueprint::s_Scale;
        const float height = m_Ascent - m_Descent + m_LineGap;
        return height * scale;
    }

    float Font::GetAscent(const int fontSize) const
    {
        const float scale = static_cast<float>(fontSize) / builder::FontBlueprint::s_Scale;
        return m_Ascent * scale;
    }

    float Font::GetDescent(const int fontSize) const
    {
        const float scale = static_cast<float>(fontSize) / builder::FontBlueprint::s_Scale;
        return m_Descent * scale;
    }
}
