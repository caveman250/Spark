#pragma once

#include "spark.h"
#include "binary/Database.h"
#include "engine/io/VFS.h"
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
                return std::static_pointer_cast<T>(asset.lock());
            }
        }

        auto db = binary::Database::Load(path, true);
        std::shared_ptr<T> asset = std::make_shared<T>();
        reflect::DeserialiseType<T>(db, *asset);
        m_AssetCache[fullPath.Data()] = asset;

        return asset;
    }
}
