#include "AssetManager.h"
#include "Asset.h"

namespace se::asset
{
    AssetManager* AssetManager::Get()
    {
        static AssetManager* s_Instance = new AssetManager();
        return s_Instance;
    }

    std::shared_ptr<Asset> AssetManager::GetAsset(const std::string& path,
                                                  reflect::Type* type)
    {
        std::lock_guard guard(m_Mutex);

        //static_assert(std::is_convertible<T*, Asset*>::value, "Attempting to load a non asset type via AssetManager::GetAsset");

        auto optionalFullPath = io::VFS::Get().ResolveFSPath(path, false);
        if (!SPARK_VERIFY(optionalFullPath.has_value()))
        {
            return nullptr;
        }

        auto& fullPath = *optionalFullPath;
        if (m_AssetCache.contains(fullPath.Data()))
        {
            auto& asset = m_AssetCache.at(fullPath.Data());
            if (!asset.expired())
            {
                return asset.lock();
            }
        }

        auto db = binary::Database::Load(path, true);
        auto root = db->GetRoot();
        std::shared_ptr<Asset> asset = std::shared_ptr<Asset>(static_cast<Asset*>(type->heap_constructor()));
        type->Deserialize(asset.get(), root, {});
        m_AssetCache[fullPath.Data()] = asset;

        return asset;
    }
}
