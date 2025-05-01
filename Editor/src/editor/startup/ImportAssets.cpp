#include "engine/io/VFS.h"
#include "engine/asset/meta/MetaData.h"
#include "engine/asset/builder/AssetBuilder.h"
#include "ImportAssets.h"

#include "engine/io/OutputFileStream.h"

namespace se::editor::startup
{
    void ImportAssets::Run()
    {
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "assets"), "/source_assets");
        io::VFS::Get().Mount(std::format("{}/{}", ENGINE_DIR, "engine_assets"), "/engine_source_assets");

        ProcessDir("/source_assets", "/assets");
        ProcessDir("/engine_source_assets", "/engine_assets");

        io::VFS::Get().Unmount("/source_assets");
        io::VFS::Get().Unmount("/engine_source_assets");
    }

    void ImportAssets::ProcessDir(const std::string& dir, const std::string& outputDir) const
    {
        io::VFS::Get().ForEachFile(dir, true, [dir, outputDir](const io::VFSFile& file)
        {
            if (se::asset::builder::AssetBuilder::IsRelevantFile(file.fullPath.Data()))
            {
                auto meta = asset::meta::MetaData::GetMetaDataForAsset(file.fullPath.Data());

                std::string outputPathBase = file.fullPath.Data();
                outputPathBase.replace(0, dir.length(), outputDir);
                auto extensionIt = outputPathBase.find_last_of(".");
                outputPathBase.replace(extensionIt, outputPathBase.length() - extensionIt, ".sass");

                if (se::asset::builder::AssetBuilder::IsOutOfDate(file.fullPath.Data(), meta.value(), outputPathBase))
                {
                    for (const auto& builtAsset : se::asset::builder::AssetBuilder::ProcessAsset(file.fullPath.Data(), outputPathBase, meta.value()))
                    {
                        if (!builtAsset.fileNameSuffix.empty())
                        {
                            auto outputPath = outputPathBase;
                            extensionIt = outputPath.find_last_of(".");
                            outputPath.insert(extensionIt, builtAsset.fileNameSuffix);
                            builtAsset.db->Save(outputPath);

                            std::string jsonPth = outputPath;
                            jsonPth = jsonPth.replace(jsonPth.length() - 5, 5, ".json");
                            io::VFS::Get().WriteText(jsonPth, builtAsset.db->ToJson().dump(4));
                        }
                        else
                        {
                            builtAsset.db->Save(outputPathBase);

                            std::string jsonPth = outputPathBase;
                            jsonPth = jsonPth.replace(jsonPth.length() - 5, 5, ".json");
                            io::VFS::Get().WriteText(jsonPth, builtAsset.db->ToJson().dump(4));
                        }
                    }
                }

                meta.value().Save();
            }
        });
    }
}
