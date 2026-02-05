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

        if (m_AssetCache.contains(path))
        {
            auto& asset = m_AssetCache.at(path);
            if (!asset.expired())
            {
                return asset.lock();
            }
        }

        auto db = binary::Database::Load(path, true);
        if (!SPARK_VERIFY(db))
        {
            return nullptr;
        }

        auto root = db->GetRoot();
        std::shared_ptr<Asset> asset = std::shared_ptr<Asset>(static_cast<Asset*>(type->heap_constructor()));
        asset->m_Path = path;
        type->Deserialize(asset.get(), root, {});
        m_AssetCache[path] = asset;

        return asset;
    }
}
