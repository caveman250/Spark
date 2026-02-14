#pragma once

#include "spark.h"
#include "Blueprint.h"
#include "engine/memory/BinaryBlob.h"

namespace se::asset {
    struct CharData;
}

struct stbtt_fontinfo;

namespace se::ui
{
    class FloatRect;
}

namespace se::asset::builder
{
    class FontBlueprint : public Blueprint
    {
    public:
        static constexpr int s_Scale = 80;
        std::regex GetFilePattern() const override;
        std::vector<BuiltAsset> BuildAsset(const std::string& path, const std::string& outputPath) const override;

    private:
        static bool LoadFont(stbtt_fontinfo& font, void** fontData, const std::string& path);
        static std::vector<std::pair<ui::FloatRect, int>> CollectSortedBoundingBoxes(stbtt_fontinfo& font, float scale);
        static void CollectCharMetrics(stbtt_fontinfo& font, char c, float scale, float ascent, CharData& charData);
        static void PackChar(ui::FloatRect FloatRect, std::vector<ui::FloatRect>& placedRects, int& imageWidth, int& imageHeight, int& scanlineDelta);
        static memory::BinaryBlob GenerateMonochromeBitmap(stbtt_fontinfo &font, int width, int height, float scale,
            const std::vector<std::pair<ui::FloatRect, int>>& boundingBoxes, const std::vector<ui::FloatRect>& placedBoundingBoxes);
    };
}
