#include "engine/asset/meta/MetaData.h"
#include "engine/io/VFS.h"
#include "AssetBuilder.h"
#include "Blueprint.h"
#include "TextureBlueprint.h"
#include "FBXBlueprint.h"
#include "FontBlueprint.h"
#include "ShaderBlueprint.h"

namespace se::asset::builder
{
    std::array<std::unique_ptr<Blueprint>, 4> s_AssetBlueprints =
    {
        std::make_unique<TextureBlueprint>(),
        std::make_unique<FBXBlueprint>(),
        std::make_unique<ShaderBlueprint>(),
        std::make_unique<FontBlueprint>()
    };

    std::vector<BuiltAsset> AssetBuilder::ProcessAsset(const std::string& assetPath, const std::string& outputPath, meta::MetaData& meta)
    {
        for (const auto& bp : s_AssetBlueprints)
        {
            if (std::regex_match(assetPath, bp->GetFilePattern()))
            {
                auto dbs = bp->BuildAsset(assetPath, outputPath, meta);
                meta.SetFormatVersion(bp->GetLatestVersion());
                return dbs;
            }
        }

        return { };
    }

    bool AssetBuilder::IsRelevantFile(const std::string &assetPath)
    {
        for (const auto& bp : s_AssetBlueprints)
        {
            if (std::regex_match(assetPath, bp->GetFilePattern()))
            {
               return true;
            }
        }

        return false;
    }

    bool AssetBuilder::IsOutOfDate(const std::string& assetPath, const meta::MetaData& meta, const std::string& outputPath)
    {
        auto& vfs = io::VFS::Get();
        if (!vfs.Exists(outputPath) || vfs.GetLastModified(outputPath) < vfs.GetLastModified(assetPath))
        {
            return true;
        }

        for (const auto& bp : s_AssetBlueprints)
        {
            if (std::regex_match(assetPath, bp->GetFilePattern()))
            {
                return bp->GetLatestVersion() > meta.GetFormatVersion();
            }
        }

        return false;
    }
}
