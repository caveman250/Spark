#include "Blueprint.h"
#include "engine/io/VFS.h"
import Spark.Asset.Meta.MetaDataManager;

namespace se::asset::builder
{
    bool Blueprint::IsOutOfDate(const std::string& assetPath, const std::string& outputPath)
    {
        const auto metaManager = meta::MetaManager::Get();
        auto& vfs = io::VFS::Get();
        return !vfs.Exists(outputPath) || vfs.GetLastModified(outputPath) < vfs.GetLastModified(metaManager->GetMetaPath(assetPath));
    }
}
