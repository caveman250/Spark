#include "MetaData.h"

#include "engine/io/VFS.h"

namespace se::asset::meta
{
    std::optional<MetaData> MetaData::GetMetaDataForAsset(const std::string& assetPath)
    {
        auto metaPath = GetMetaPath(assetPath);
        if (metaPath.empty())
        {
            return std::nullopt;
        }

        auto ret = CreateMetaDataForAsset(assetPath);
        if (!io::VFS::Get().Exists(metaPath))
        {
            return ret;
        }
        else
        {
            std::string metaData = io::VFS::Get().ReadText(metaPath);
            nlohmann::json json = nlohmann::json::parse(metaData);
            ret.FromJson(json);
            return ret;
        }
    }

    MetaData MetaData::CreateMetaDataForAsset(const std::string& assetPath)
    {
        return MetaData(assetPath); // assuming this will get more complicated?
    }

    void MetaData::Save()
    {
        nlohmann::json saveData = ToJson();
        io::VFS::Get().WriteText(GetMetaPath(m_AssetPath), saveData.dump());
    }

    MetaData::MetaData(const std::string& assetPath)
        : m_AssetPath(assetPath)
    {
    }

    nlohmann::json MetaData::ToJson()
    {
        nlohmann::json meta;
        return meta;
    }

    void MetaData::FromJson(const nlohmann::json&)
    {
    }

    std::string MetaData::GetMetaPath(const std::string& assetPath)
    {
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
}
