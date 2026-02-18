#pragma once

#include "spark.h"
#include "binary/Database.h"
#include "engine/reflect/Util.h"
#include "util/AssetUtil.h"
#include "engine/io/VFS.h"

namespace se::ecs
{
    class SceneSaveData;
}

namespace se::render
{
    class Material;
}

namespace se::asset
{
    class Asset;
}

template <typename T>
concept DataAsset = std::is_same_v<T, se::render::Material> || std::is_same_v<T, se::ecs::SceneSaveData>;

namespace se::asset
{
    class AssetManager
    {
    public:
        static AssetManager* Get();

        template <typename T>
        std::shared_ptr<T> GetAsset(const std::string& path);

        std::shared_ptr<Asset> GetAsset(const std::string& path, reflect::Type* type);

        template <DataAsset T>
        std::shared_ptr<T> CreateDataAsset(const std::string& path);

#if SPARK_EDITOR
        void ForceReloadAsset(const std::string& path, reflect::Type* type);
#endif

    private:
        std::unordered_map<std::string, std::weak_ptr<Asset>> m_AssetCache;
        std::mutex m_Mutex;
    };

    template <typename T>
    std::shared_ptr<T> AssetManager::GetAsset(const std::string& path)
    {
        std::lock_guard guard(m_Mutex);

        static_assert(std::is_convertible<T*, Asset*>::value, "Attempting to load a non asset type via AssetManager::GetAsset");
        const auto it = m_AssetCache.find(path);
        if (it != m_AssetCache.end())
        {
            auto& asset = m_AssetCache.at(path);
            if (!asset.expired())
            {
                return std::static_pointer_cast<T>(asset.lock());
            }
        }

        const auto db = binary::Database::Load(path, true);
        if (!SPARK_VERIFY(db))
        {
            return nullptr;
        }
        std::shared_ptr<T> asset = std::make_shared<T>();
        reflect::DeserialiseType<T>(db, *asset);
        asset->m_Path = path;
        m_AssetCache.insert(std::make_pair(path,asset));

        return asset;
    }

    template<DataAsset T>
    std::shared_ptr<T> AssetManager::CreateDataAsset(const std::string& path)
    {
        std::lock_guard guard(m_Mutex);

        auto it = m_AssetCache.find(path);
        if (!SPARK_VERIFY(it == m_AssetCache.end()))
        {
            return nullptr;
        }

        auto sourcePath = util::GetSourcePath(path, ".json");
        std::shared_ptr<T> asset = std::make_shared<T>();
        asset->m_Path = path;
        asset->m_SourcePath = sourcePath;
        m_AssetCache.insert(std::make_pair(path,asset));

        auto db = reflect::SerialiseType<T>(asset.get());
        db->Save(path);
        io::VFS::Get().WriteText(sourcePath, db->ToJson().dump(4));

        return asset;
    }
}
