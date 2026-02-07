#include "engine/asset/meta/MetaData.h"
#include "engine/io/VFS.h"
#include "AssetBuilder.h"
#include "Blueprint.h"
#include "TextureBlueprint.h"
#include "FBXBlueprint.h"
#include "FontBlueprint.h"
#include "JsonBlueprint.h"
#include "ShaderBlueprint.h"
#include "engine/string/util/StringUtil.h"

namespace se::asset::builder
{
    std::array<std::unique_ptr<Blueprint>, 5> s_AssetBlueprints =
    {
        std::make_unique<TextureBlueprint>(),
        std::make_unique<FBXBlueprint>(),
        std::make_unique<ShaderBlueprint>(),
        std::make_unique<FontBlueprint>(),
        std::make_unique<JsonBlueprint>()
    };

    bool AssetBuilder::IsRelevantFile(const std::string &assetPath)
    {
        std::string lowerAssetPath = string::util::ToLower(assetPath);
        for (const auto& bp : s_AssetBlueprints)
        {
            if (std::regex_match(lowerAssetPath, bp->GetFilePattern()))
            {
               return true;
            }
        }

        return false;
    }

    bool AssetBuilder::IsOutOfDate(const std::string& assetPath, Blueprint* bp, const std::string& outputPath)
    {
        auto& vfs = io::VFS::Get();
        if (!vfs.Exists(outputPath) || vfs.GetLastModified(outputPath) < vfs.GetLastModified(assetPath))
        {
            return true;
        }

        if (bp->IsOutOfDate(assetPath, outputPath))
        {
            return true;
        }

        return false;
    }

    Blueprint* AssetBuilder::GetBlueprintForAsset(const std::string& assetPath)
    {
        for (const auto& bp : s_AssetBlueprints)
        {
            std::string lowerAssetPath = string::util::ToLower(assetPath);
            if (std::regex_match(lowerAssetPath, bp->GetFilePattern()))
            {
                return bp.get();
            }
        }

        return nullptr;
    }

    void AssetBuilder::ProcessAsset(const std::string& path, const std::string& outputPath)
    {
        auto* bp = asset::builder::AssetBuilder::GetBlueprintForAsset(path);

        debug::Log::Info("Processing asset {}...", path);
        for (const auto& builtAsset : bp->BuildAsset(path, outputPath))
        {
            if (!builtAsset.fileNameSuffix.empty())
            {
                auto newOutputPath = outputPath;
                auto extensionIt = newOutputPath.find_last_of(".");
                newOutputPath.insert(extensionIt, builtAsset.fileNameSuffix);
                builtAsset.db->Save(newOutputPath);

                // std::string jsonPth = outputPath;
                // jsonPth = jsonPth.replace(jsonPth.length() - 5, 5, ".json");
                // io::VFS::Get().WriteText(jsonPth, builtAsset.db->ToJson().dump(4));
            }
            else
            {
                builtAsset.db->Save(outputPath);

                // std::string jsonPth = outputPathBase;
                // jsonPth = jsonPth.replace(jsonPth.length() - 5, 5, ".json");
                // io::VFS::Get().WriteText(jsonPth, builtAsset.db->ToJson().dump(4));
            }
        }
    }
}
