#pragma once

#include "spark.h"
#include "engine/asset/Asset.h"
#include "engine/asset/AssetReference.h"
#include "engine/math/Vec2.h"
#include "engine/ui/FloatRect.h"
#include "engine/asset/texture/Texture.h"

struct stbtt_fontinfo;

namespace se::asset::builder
{
    class FontBlueprint;
}

namespace se::asset
{
    struct CharData
    {
        SPARK_POD_CLASS()

        SPARK_MEMBER(Serialized)
        float yOffset = 0;

        SPARK_MEMBER(Serialized)
        float advanceWidth;

        SPARK_MEMBER(Serialized)
        float leftSideBearing;

        SPARK_MEMBER(Serialized)
        std::unordered_map<char, float> kerning;

        SPARK_MEMBER(Serialized)
        ui::FloatRect rect;

        SPARK_MEMBER(Serialized)
        math::Vec2 uvTopLeft;

        SPARK_MEMBER(Serialized)
        math::Vec2 uvBottomRight;
    };

    class Font : public Asset
    {
        SPARK_CLASS()

        Font() = default;
        const std::string& GetName() const;
        const std::shared_ptr<Texture>& GetTextureAsset(int fontSize);
        const CharData& GetCharData(char c, int fontSize) const;
        size_t GetNumChars() const { return m_SDFCharData.size(); }
        float GetLineHeight(int fontSize) const;
        float GetAscent(int fontSize) const;
        float GetDescent(int fontSize) const;

    private:
        SPARK_MEMBER(Serialized)
        std::string m_Name;

        SPARK_MEMBER(Serialized)
        AssetReference<Texture> m_SDFTexture = {};

        SPARK_MEMBER(Serialized)
        std::vector<AssetReference<Texture>> m_BitmapTextures = {};

        SPARK_MEMBER(Serialized)
        std::unordered_map<int, std::unordered_map<char, CharData>> m_BitmapCharData = {};

        SPARK_MEMBER(Serialized)
        std::unordered_map<char, CharData> m_SDFCharData = {};

        SPARK_MEMBER(Serialized)
        float m_Ascent = 0.f;

        SPARK_MEMBER(Serialized)
        float m_Descent = 0.f;

        SPARK_MEMBER(Serialized)
        float m_LineGap = 0.f;

        friend class builder::FontBlueprint;
    };
}
