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

                            std::string jsonPth = outputPath;
                            jsonPth = jsonPth.replace(jsonPth.length() - 5, 5, ".json");
                            io::VFS::Get().WriteText(jsonPth, builtAsset.db->ToJson().dump());
                        }
                        else
                        {
                            builtAsset.db->Save(outputPathBase);

                            std::string jsonPth = outputPathBase;
                            jsonPth = jsonPth.replace(jsonPth.length() - 5, 5, ".json");
                            io::VFS::Get().WriteText(jsonPth, builtAsset.db->ToJson().dump());
                        }
                    }
                }

                meta.value().Save();
            }
        });
    }
}
