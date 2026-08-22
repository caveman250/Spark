#pragma once

#include "spark.h"
#include "Blueprint.h"

struct stbtt_fontinfo;

namespace se::asset::builder
{
    class FontBlueprint : public Blueprint
    {
    public:
        static constexpr int SDFPadding = 16;
        static constexpr int Scale = 128;
        static constexpr int BitmapCutoffSize = 20.f;

        std::regex GetFilePattern() const override;
        std::vector<BuiltAsset> BuildAsset(const std::string& path, const std::string& outputPath) const override;
    };
}
