#include "engine/io/VFS.h"
#include "engine/asset/meta/MetaData.h"
#include "engine/asset/builder/AssetBuilder.h"
#include "ImportAssets.h"

#include "engine/asset/AssetManager.h"
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
            if (asset::builder::AssetBuilder::IsRelevantFile(file.fullPath))
            {
                std::string outputPath = asset::util::GetAssetBuiltPath(file.fullPath);
                auto* bp = asset::builder::AssetBuilder::GetBlueprintForAsset(file.fullPath);

                if (asset::builder::AssetBuilder::IsOutOfDate(file.fullPath, bp, outputPath))
                {
                    asset::builder::AssetBuilder::ProcessAsset(file.fullPath, outputPath);
                }
            }
        }, true);
    }
}
