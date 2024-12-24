#include "Font.h"
#include "stb_truetype.h"
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
    DEFINE_SPARK_CLASS_END()

    DEFINE_SPARK_POD_CLASS_BEGIN(FontAssetData)
        DEFINE_SERIALIZED_MEMBER(path)
        DEFINE_SERIALIZED_MEMBER(charData)
    DEFINE_SPARK_CLASS_END()

    DEFINE_SPARK_CLASS_BEGIN(Font)
        DEFINE_SERIALIZED_MEMBER(m_Name)
        DEFINE_SERIALIZED_MEMBER(m_AssetData)
    DEFINE_SPARK_CLASS_END()

    std::shared_ptr<Texture> Font::GetTextureAsset(int fontSize)
    {
        if (SPARK_VERIFY(m_AssetData.contains(fontSize)))
        {
            return AssetManager::Get()->GetAsset<Texture>(m_AssetData.at(fontSize).path);
        }

        return nullptr;
    }

    CharData Font::GetCharData(int fontSize, char c) const
    {
        if (!SPARK_VERIFY(m_AssetData.contains(fontSize)))
        {
            return {};
        }

        auto fontAsset = m_AssetData.at(fontSize);
        if (!SPARK_VERIFY(fontAsset.charData.contains(c)))
        {
            return {};
        }

        return fontAsset.charData.at(c);
    }
}
