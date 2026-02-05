#include "MetaDataManager.h"

namespace se::asset::meta
{
    MetaManager* MetaManager::Get()
    {
        static auto s_Instance = new MetaManager();
        return s_Instance;
    }

    std::string MetaManager::GetMetaPath(std::string assetPath)
    {
        if (assetPath.ends_with(".sass"))
        {
            assetPath = util::GetAssetSourcePath(assetPath);
        }

        auto metaPath = assetPath;
        auto extensionIndex = metaPath.find_last_of('.');
        if (extensionIndex != std::string::npos)
        {
            auto first = metaPath.find(".");
            auto size = metaPath.size() - first;
            metaPath.replace(first, first + size, ".meta");
        }
        else
        {
            debug::Log::Error("MetaData::GetMetaDataForAsset - invalid filename {}.", assetPath);
            return {};
        }

        return metaPath;
    }

    void MetaManager::SaveMetaData(const std::shared_ptr<MetaData>& meta)
    {
        auto type = meta->GetReflectType();
        std::string metaPath = GetMetaPath(meta->GetAssetPath());

        auto db = binary::Database::Create(false);
        db->SetRootStruct(db->GetOrCreateStruct(type->GetTypeName(nullptr), type->GetStructLayout(nullptr)));
        auto root = db->GetRoot();
        type->Serialize(meta.get(), root, {});
        io::VFS::Get().WriteText(metaPath, db->ToJson().dump(4));
    }

    void MetaManager::InitMetaData(const std::shared_ptr<MetaData>& meta)
    {
        auto type = meta->GetReflectType();
        std::string metaPath = GetMetaPath(meta->GetAssetPath());
        if (!io::VFS::Get().Exists(metaPath))
        {
            SaveMetaData(meta);
        }
        else
        {
            std::string metaData = io::VFS::Get().ReadText(metaPath);
            nlohmann::json json = nlohmann::json::parse(metaData.data());
            auto db = binary::Database::FromJson(json);
            auto root = db->GetRoot();
            type->Deserialize(meta.get(), root, {});
        }

        m_MetaCache.insert(std::make_pair(meta->GetAssetPath(), meta));
    }

    std::shared_ptr<MetaData> MetaManager::GetOrCreateMetaDataForAsset(Asset* asset)
    {
        auto it = m_MetaCache.find(asset->m_Path);
        if (it != m_MetaCache.end())
        {
            return it->second;
        }

        auto metaPath = GetMetaPath(asset->m_Path);
        if (metaPath.empty())
        {
            return nullptr;
        }

        auto ret = asset->CreateMetaData();
        InitMetaData(ret);

        return ret;
    }
}
