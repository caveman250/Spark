#include <engine/asset/meta/MetaData.h>
#include <engine/io/VFS.h>
#include "AssetBuilder.h"
#include "Blueprint.h"
#include "TextureBlueprint.h"
#include "FBXBlueprint.h"
#include "ShaderBlueprint.h"
#include "engine/io/OutputFileStream.h"

namespace se::asset::builder
{
    std::array<std::unique_ptr<Blueprint>, 3> s_AssetBlueprints =
    {
        std::make_unique<TextureBlueprint>(),
        std::make_unique<FBXBlueprint>(),
        std::make_unique<ShaderBlueprint>()
    };

    std::shared_ptr<binary::Database> AssetBuilder::ProcessAsset(const std::string& assetPath, meta::MetaData& meta)
    {
        for (const auto& bp : s_AssetBlueprints)
        {
            if (std::regex_match(assetPath, bp->GetFilePattern()))
            {
                auto db = bp->BuildAsset(assetPath, meta);
                meta.SetFormatVersion(bp->GetLatestVersion());
                return db;
            }
        }

        return nullptr;
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
