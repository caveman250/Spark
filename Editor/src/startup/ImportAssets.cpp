#include "engine/io/VFS.h"
#include "engine/asset/meta/MetaData.h"
#include "engine/asset/builder/AssetBuilder.h"
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

                std::string outputPathBase = path;
                outputPathBase.replace(0, dir.length(), outputDir);
                auto extensionIt = outputPathBase.find_last_of(".");
                outputPathBase.replace(extensionIt, outputPathBase.length() - extensionIt, ".sass");

                if (se::asset::builder::AssetBuilder::IsOutOfDate(path, meta.value(), outputPathBase))
                {
                    for (const auto& builtAsset : se::asset::builder::AssetBuilder::ProcessAsset(path, outputPathBase, meta.value()))
                    {
                        if (!builtAsset.fileNameSuffix.empty())
                        {
                            auto outputPath = outputPathBase;
                            extensionIt = outputPath.find_last_of(".");
                            outputPath.insert(extensionIt, builtAsset.fileNameSuffix);
                            builtAsset.db->Save(outputPath);
                        }
                        else
                        {
                            builtAsset.db->Save(outputPathBase);

                            // io::OutputFileStream fs(outputPathBase + ".json", false);
                            // fs << builtAsset.db->ToJson().dump();
                            // fs.Close();
                        }
                    }
                }

                meta.value().Save();
            }
        });
    }
}
