#include "AssetBuilder.h"
#include "Blueprint.h"
#include "TextureBlueprint.h"

namespace se::asset::builder
{
    std::array<std::unique_ptr<Blueprint>, 1> s_AssetBlueprints =
    {
        std::make_unique<TextureBlueprint>()
    };

    std::shared_ptr<binary::Database> AssetBuilder::ProcessAsset(const std::string& assetPath)
    {
        for (const auto& bp : s_AssetBlueprints)
        {
            if (std::regex_match(assetPath, bp->GetFilePattern()))
            {
                return bp->BuildAsset(assetPath);
            }
        }

        return nullptr;
    }
}