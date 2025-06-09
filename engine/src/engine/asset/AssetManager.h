#pragma once

#include "spark.h"
#include "binary/Database.h"
#include "engine/reflect/Util.h"

namespace se::asset
{
    class Asset;
}

namespace se::asset
{
    class AssetManager 
    {
    public:
        static AssetManager* Get();

        template <typename T>
        std::shared_ptr<T> GetAsset(const std::string& path);

        std::shared_ptr<Asset> GetAsset(const std::string& path, reflect::Type* type);

    private:
        std::unordered_map<std::string, std::weak_ptr<Asset>> m_AssetCache;
        std::mutex m_Mutex;
    };

    template <typename T>
    std::shared_ptr<T> AssetManager::GetAsset(const std::string& path)
    {
        std::lock_guard guard(m_Mutex);

        static_assert(std::is_convertible<T*, Asset*>::value, "Attempting to load a non asset type via AssetManager::GetAsset");
        if (m_AssetCache.contains(path))
        {
            auto& asset = m_AssetCache.at(path);
            if (!asset.expired())
            {
                return std::static_pointer_cast<T>(asset.lock());
            }
        }

        auto db = binary::Database::Load(path, true);
        if (!SPARK_VERIFY(db))
        {
            return nullptr;
        }
        std::shared_ptr<T> asset = std::make_shared<T>();
        reflect::DeserialiseType<T>(db, *asset);
        asset->m_Path = path;
        m_AssetCache[path] = asset;

        return asset;
    }
}
