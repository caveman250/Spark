#include <engine/io/VFS.h>
#include <engine/asset/meta/MetaData.h>
#include <engine/asset/builder/AssetBuilder.h>
#include "ImportAssets.h"

namespace se::editor::startup
{
    void ImportAssets::Run()
    {
        io::VFS::Get().ForEachFile("/source_assets", true, [](const std::string& path)
        {
            if (se::asset::builder::AssetBuilder::IsRelevantFile(path))
            {
                auto meta = asset::meta::MetaData::GetMetaDataForAsset(path);

                // todo meta options go here

                std::string outputPath = path;
                outputPath.replace(0, 14 /* strlen("/source_assets") */, "/assets");
                auto extensionIt = outputPath.find_last_of(".");
                outputPath.replace(extensionIt, outputPath.length() - extensionIt, ".sass");

                if (se::asset::builder::AssetBuilder::IsOutOfDate(path, meta.value(), outputPath))
                {
                    auto db = se::asset::builder::AssetBuilder::ProcessAsset(path, meta.value());
                    db->Save(outputPath);
                }

                meta.value().Save();
            }
        });
    }
}
