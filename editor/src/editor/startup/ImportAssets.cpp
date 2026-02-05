#include "engine/io/VFS.h"
#include "engine/asset/meta/MetaData.h"
#include "engine/asset/builder/AssetBuilder.h"
#include "ImportAssets.h"

#include "engine/asset/meta/MetaDataManager.h"

namespace se::editor::startup
{
    void ImportAssets::Run()
    {
        ProcessDir("/source_assets", "/assets");
        ProcessDir("/engine_source_assets", "/engine_assets");
    }

    void ImportAssets::ProcessDir(const std::string& dir, const std::string& outputDir) const
    {
        io::VFS::Get().ForEachFile(dir, true, [dir, outputDir](const io::VFSFile& file)
        {
            if (se::asset::builder::AssetBuilder::IsRelevantFile(file.fullPath.data()))
            {
                std::string outputPathBase = file.fullPath.data();
                outputPathBase.replace(0, dir.length(), outputDir);
                auto extensionIt = outputPathBase.find_last_of(".");
                outputPathBase.replace(extensionIt, outputPathBase.length() - extensionIt, ".sass");

                auto* bp = asset::builder::AssetBuilder::GetBlueprintForAsset(file.fullPath);

                if (se::asset::builder::AssetBuilder::IsOutOfDate(file.fullPath, bp, outputPathBase))
                {
                    debug::Log::Info("Processing asset {}...", file.fullPath);
                    for (const auto& builtAsset : bp->BuildAsset(file.fullPath, outputPathBase))
                    {
                        if (!builtAsset.fileNameSuffix.empty())
                        {
                            auto outputPath = outputPathBase;
                            extensionIt = outputPath.find_last_of(".");
                            outputPath.insert(extensionIt, builtAsset.fileNameSuffix);
                            builtAsset.db->Save(outputPath);

                            // std::string jsonPth = outputPath;
                            // jsonPth = jsonPth.replace(jsonPth.length() - 5, 5, ".json");
                            // io::VFS::Get().WriteText(jsonPth, builtAsset.db->ToJson().dump(4));
                        }
                        else
                        {
                            builtAsset.db->Save(outputPathBase);

                            // std::string jsonPth = outputPathBase;
                            // jsonPth = jsonPth.replace(jsonPth.length() - 5, 5, ".json");
                            // io::VFS::Get().WriteText(jsonPth, builtAsset.db->ToJson().dump(4));
                        }
                    }
                }
            }
        }, true);
    }
}
