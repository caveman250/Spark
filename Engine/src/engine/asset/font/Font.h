#pragma once

#include "spark.h"
#include "engine/asset/Asset.h"
#include "engine/math/Vec2.h"
#include "engine/ui/Rect.h"

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

        int yOffset = 0;
        int advanceWidth;
        int leftSideBearing;
        std::unordered_map<char, int> kerning;
        ui::Rect rect;
        math::Vec2 uvTopLeft;
        math::Vec2 uvBottomRight;
    };

    struct FontAssetData
    {
        DECLARE_SPARK_POD_CLASS(FontAssetData)

        std::string path;
        std::unordered_map<char, CharData> charData = {};
    };

    class Font : public Asset
    {
        DECLARE_SPARK_CLASS(Font)

        Font() = default;
        std::shared_ptr<Texture> GetTextureAsset(int fontSize);
        const CharData& GetCharData(int fontSize, char c) const;

    private:
        std::string m_Name;
        std::unordered_map<int, FontAssetData> m_AssetData = {};

        friend class builder::FontBlueprint;
    };
}
