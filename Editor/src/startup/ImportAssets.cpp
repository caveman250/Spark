#include <engine/io/VFS.h>
#include <engine/asset/meta/MetaData.h>
#include <engine/asset/builder/AssetBuilder.h>
#include "ImportAssets.h"

#include "engine/io/OutputFileStream.h"

namespace se::editor::startup
{
    void ImportAssets::Run()
    {
        ProcessDir("/source_assets", "/assets");
        ProcessDir("/builtin_source_assets", "/builtin_assets");
    }

    void ImportAssets::ProcessDir(const std::string& dir, const std::string& outputDir) const
    {
        io::VFS::Get().ForEachFile(dir, true, [dir, outputDir](const std::string& path)
        {
            if (se::asset::builder::AssetBuilder::IsRelevantFile(path))
            {
                auto meta = asset::meta::MetaData::GetMetaDataForAsset(path);

                // todo meta options go here

                std::string outputPath = path;
                outputPath.replace(0, dir.length(), outputDir);
                auto extensionIt = outputPath.find_last_of(".");
                outputPath.replace(extensionIt, outputPath.length() - extensionIt, ".sass");

                if (se::asset::builder::AssetBuilder::IsOutOfDate(path, meta.value(), outputPath))
                {
                    if (auto db = se::asset::builder::AssetBuilder::ProcessAsset(path, meta.value()))
                        db->Save(outputPath);
                    // io::OutputFileStream fs(outputPath + ".json", false);
                    // fs << db->ToJson().dump();
                    // fs.Close();
                }

                meta.value().Save();
            }
        });
    }
}
