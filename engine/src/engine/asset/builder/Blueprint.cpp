#include "Blueprint.h"
#include "engine/asset/meta/MetaDataManager.h"

namespace se::asset::builder
{
    bool Blueprint::IsOutOfDate(const std::string& assetPath, const std::string& outputPath)
    {
        const auto metaManager = meta::MetaManager::Get();
        auto& vfs = io::VFS::Get();
        return !vfs.Exists(outputPath) || vfs.GetLastModified(outputPath) < vfs.GetLastModified(metaManager->GetMetaPath(assetPath));
    }
}
