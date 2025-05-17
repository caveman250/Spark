#include "Font.h"
#include "engine/asset/AssetManager.h"
#include "engine/asset/texture/Texture.h"

namespace se::asset
{
    DEFINE_SPARK_POD_CLASS_BEGIN(CharData)
        DEFINE_SERIALIZED_MEMBER(yOffset)
        DEFINE_SERIALIZED_MEMBER(advanceWidth)
        DEFINE_SERIALIZED_MEMBER(leftSideBearing)
        DEFINE_SERIALIZED_MEMBER(kerning)
        DEFINE_SERIALIZED_MEMBER(rect)
        DEFINE_SERIALIZED_MEMBER(uvTopLeft)
        DEFINE_SERIALIZED_MEMBER(uvBottomRight)
    DEFINE_SPARK_CLASS_END(CharData)

    DEFINE_SPARK_CLASS_BEGIN(Font)
        DEFINE_SERIALIZED_MEMBER(m_Name)
        DEFINE_SERIALIZED_MEMBER(m_Texture)
        DEFINE_SERIALIZED_MEMBER(m_CharData)
        DEFINE_SERIALIZED_MEMBER(m_Ascent)
        DEFINE_SERIALIZED_MEMBER(m_Descent)
    DEFINE_SPARK_CLASS_END(Font)

    const std::string& Font::GetName() const
    {
        return m_Name;
    }

    std::shared_ptr<Texture> Font::GetTextureAsset()
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
