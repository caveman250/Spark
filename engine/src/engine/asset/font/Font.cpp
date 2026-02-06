#include "Font.h"
#include "engine/asset/AssetManager.h"
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

    const CharData& Font::GetCharData(char c) const
    {
        if (!SPARK_VERIFY(m_CharData.contains(c)))
        {
            static CharData nullCharData = {};
            return nullCharData;
        }

        return m_CharData.at(c);
    }

    float Font::GetLineHeight(int fontSize) const
    {
        float scale = static_cast<float>(fontSize) / 32.f;
        float height = m_Ascent - m_Descent + m_LineGap;
        return height * scale;
    }

    float Font::GetAscent(int fontSize) const
    {
        float scale = static_cast<float>(fontSize) / 32.f;
        return m_Ascent * scale;
    }

    float Font::GetDescent(int fontSize) const
    {
        float scale = static_cast<float>(fontSize) / 32.f;
        return m_Descent * scale;
    }
}
