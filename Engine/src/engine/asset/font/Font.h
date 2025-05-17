#pragma once

#include "spark.h"
#include "engine/asset/Asset.h"
#include "engine/math/Vec2.h"
#include "engine/ui/FloatRect.h"

struct stbtt_fontinfo;

namespace se::asset {
    class Texture;
}

namespace se::asset::builder
{
    class FontBlueprint;
}

namespace se::asset
{
    struct CharData
    {
        DECLARE_SPARK_POD_CLASS(CharData)

        float yOffset = 0;
        float advanceWidth;
        float leftSideBearing;
        std::unordered_map<char, float> kerning;
        ui::FloatRect rect;
        math::Vec2 uvTopLeft;
        math::Vec2 uvBottomRight;
    };

    class Font : public Asset
    {
        DECLARE_SPARK_CLASS(Font)

        Font() = default;
        std::shared_ptr<Texture> GetTextureAsset();
        const CharData& GetCharData(char c) const;
        float GetLineHeight(int fontSize) const;
        float GetAscent(int fontSize) const;

    private:
        std::string m_Name;
        std::shared_ptr<Texture> m_Texture = nullptr;
        std::unordered_map<char, CharData> m_CharData = {};
        float m_Ascent = 0.f;
        float m_Descent = 0.f;
        float m_LineGap = 0.f;

        friend class builder::FontBlueprint;
    };
}
